#pragma once

#include "core.h"
#include "buffer.h"
#include "chunker.h"
#include "file_manager.h"
#include "memory_controller.h"

#pragma push_macro("guard")
#pragma push_macro("unguard")
#undef guard
#undef unguard
#include <tbb/tbb.h>
#pragma pop_macro("guard")
#pragma pop_macro("unguard")


class PipelineStageBase {
public:
    virtual ~PipelineStageBase() = default;
    virtual std::any process(const std::string& stage_name, const std::any& input) = 0;
    
};


template <typename In, typename Out>
class PipelineStage : public PipelineStageBase {
public:
    std::any process(const std::string& stage_name, const std::any& input) override {
        if (input.type() != typeid(std::shared_ptr<In>)) {
            fmt::print(
                "Error in stage '{}': Expected input of type '{}', but got '{}'\n",
                stage_name, typeid(std::shared_ptr<In>).name(), input.type().name()
            );
            return std::any();
        }
        try {
            auto in_ptr = std::any_cast<std::shared_ptr<In>>(input);
            return process_stage(in_ptr);
        } catch (const std::bad_any_cast& e) {
            fmt::print("Error in stage '{}': {}\n", stage_name, e.what());
            return std::any();
        }
    }

    virtual std::shared_ptr<Out> process_stage(const std::shared_ptr<In>& input) = 0;

};


class FileReaderToStringBuffer : public PipelineStage<std::string, StringBuffer> {
public:
    FileReaderToStringBuffer(const std::shared_ptr<FileManager>& file_manager)
        : __file_manager(file_manager) {}

    std::shared_ptr<StringBuffer> process_stage(const std::shared_ptr<std::string>& file_path) override {
        auto file_content = __file_manager->read_file(*file_path);
        if (file_content.has_value()) {
            return std::make_shared<StringBuffer>(file_content.value());
        } else {
            fmt::print("Error reading file: {}\n", file_content.error());
            return nullptr;
        }
    }

private:
    std::shared_ptr<FileManager> __file_manager;

};


template <typename T>
class StringBufferToChunkBuffer : public PipelineStage<StringBuffer, SafeChunkBuffer<T>> {
public:
    StringBufferToChunkBuffer(std::shared_ptr<Chunker> chunker = nullptr)
        : __chunker(chunker) {
        if (!__chunker) {
            __chunker = std::make_shared<NaiveChunker>(SENTENCES_PER_CHUNK);
        }
    }

    std::shared_ptr<SafeChunkBuffer<T>> process_stage(const std::shared_ptr<StringBuffer>& string_buffer) override {
        std::string data = string_buffer->get_data();
        auto result = __chunker->process(data);
        if (result.has_value()) {
            auto chunk_buffer = std::make_shared<SafeChunkBuffer<T>>();
            chunk_buffer->set_data(result.value());
            return chunk_buffer;
        } else {
            fmt::print("Error during chunking: {}\n", result.error());
            return nullptr;
        }
    }

private:
    std::shared_ptr<Chunker> __chunker;

};


template <typename T>
class ChunkBufferToPrint : public PipelineStage<SafeChunkBuffer<T>, void> {
public:
    ChunkBufferToPrint() {}

    std::shared_ptr<void> process_stage(const std::shared_ptr<SafeChunkBuffer<T>>& chunk_buffer) override {
        auto chunks = chunk_buffer->get_data();
        if (!chunks.empty()) { fmt::print("{}\n", chunks[1]); }
        else { fmt::print("Error: No chunks to print.\n"); }
        return std::make_shared<int>(1);
    }

};


template <typename T>
class ChunkBufferToEmbeddings : public PipelineStage<SafeChunkBuffer<T>, void> {
public:
    ChunkBufferToEmbeddings(const std::shared_ptr<MemoryController>& memory_controller)
        : __memory_controller(memory_controller) {}

    std::shared_ptr<void> process_stage(const std::shared_ptr<SafeChunkBuffer<T>>& chunk_buffer) override {
        __memory_controller->process_chunks(*chunk_buffer);
        return std::make_shared<int>(1);
    }

private:
    std::shared_ptr<MemoryController> __memory_controller;

};


class PipelineFactory {
public:
    template<typename Stage, typename... Args>
    void register_stage(const std::string& stage_name, Args... args) {
        __creators[stage_name] = [args...]() { return std::make_shared<Stage>(args...); };
    }

    std::shared_ptr<PipelineStageBase> create_stage(const std::string& stage_name) {
        auto it = __creators.find(stage_name);
        if (it != __creators.end()) { return it->second(); }
        return nullptr;
    }

private:
    std::unordered_map<std::string, std::function<std::shared_ptr<PipelineStageBase>()>> __creators;

};


class PipelineStageNode {
public:
    PipelineStageNode(const std::string& name) : stage_name(name), next(nullptr) {}
    std::string stage_name;
    std::shared_ptr<PipelineStageNode> next;

};

/// TODO: Refine to meta reflection

template <typename T>
struct is_vector_type : std::false_type {};

template <typename T, typename A>
struct is_vector_type<std::vector<T, A>> : std::true_type {};

bool is_vector(const std::any& input) {
    const std::type_info& type = input.type();
    if (type == typeid(std::vector<int>) ||
        type == typeid(std::vector<float>) ||
        type == typeid(std::vector<double>) ||
        type == typeid(std::vector<char>) ||
        type == typeid(std::vector<std::string>) ||
        type == typeid(std::vector<std::shared_ptr<std::string>>)) {
        return true;
    }
    return false;
}

class Pipeline {
public:
    Pipeline(PipelineFactory& factory) : __factory(factory), head(nullptr), tail(nullptr) {}

    Pipeline& stage(const std::string& stage_name) {
        auto node = std::make_shared<PipelineStageNode>(stage_name);
        if (!head) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        return *this;
    }

    using ResultHandler = std::function<void(const std::any&, const std::any&)>;
    Pipeline& result_handler_async(ResultHandler handler) {
        result_handler = std::move(handler);
        return *this;
    }

/*
    std::any execute(const std::any& input) {
        std::any intermediate = input;
        auto current = head;
        while (current) {
            auto stage = __factory.create_stage(current->stage_name);
            if (!stage) {
                fmt::print("Error: Unknown stage name {}\n", current->stage_name);
                return std::any();
            }
            intermediate = stage->process(current->stage_name, intermediate);
            if (intermediate.has_value()) {
                if (intermediate.type() == typeid(std::shared_ptr<void>) 
                    && !std::any_cast<std::shared_ptr<void>>(intermediate)) {
                    fmt::print("Pipeline stage '{}' failed. Stopping pipeline.\n", current->stage_name);
                    return std::any();
                }
            } else {
                fmt::print("Pipeline stage '{}' failed. Stopping pipeline.\n", current->stage_name);
                return std::any();
            }
            current = current->next;
        }
        return intermediate;
    }
*/

//// TODO: Refone to meta resolver
  std::any execute(const std::any& input) {
    std::any intermediate = input;
    auto current = head;
    while (current) {
        /// TODO: use ObjectFactory
        auto stage = __factory.create_stage(current->stage_name);
        if (!stage) {
            fmt::print("Error: Unknown stage name {}\n", current->stage_name);
            return std::any();
        }

        ///bool b_is_vector = intermediate.type() == typeid(std::vector<std::any>);

        bool b_is_vector = is_vector(intermediate);
        std::cout << b_is_vector << "\n\n";

        if (current->next) {

            size_t size = 1;

            if (b_is_vector) {
                const auto& vec = std::any_cast<const std::vector<std::shared_ptr<std::string>>&>(intermediate);
                size = vec.size();
                std::cout << size << "\n\n";
            }

            std::vector<std::any> intermediate_results(size);

            /// Keep tbb 

            tbb::parallel_for(tbb::blocked_range<size_t>(0, size),
                [&](const tbb::blocked_range<size_t>& range) {
                    for (size_t i = range.begin(); i != range.end(); ++i) {
                        if (b_is_vector) {
                            const auto& vec = std::any_cast<const std::vector<std::shared_ptr<std::string>>&>(intermediate);
                            intermediate_results[i] = stage->process(current->stage_name, vec[i]);
                        } else {
                            intermediate_results[i] = stage->process(current->stage_name, intermediate);
                        }
                    }
                }
            );

            if (b_is_vector) {
                intermediate = intermediate_results;
            } else {
                intermediate = intermediate_results[0];
            }
        } else {
            intermediate = stage->process(current->stage_name, intermediate);
        }

        if (!intermediate.has_value()) {
            fmt::print("Pipeline stage '{}' failed. Stopping pipeline.\n", current->stage_name);
            return std::any();
        }

        current = current->next;
    }
    return intermediate;
}


    void execute_async(const std::any& input) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        auto future = std::async(std::launch::async, [this, input]() {
            auto result = this->execute(input);
            if (result_handler) {
                result_handler(input, result);
            }
            return result;
        });
        async_queue.push_back(std::move(future));
    }

private:
    PipelineFactory& __factory;
    std::shared_ptr<PipelineStageNode> head;
    std::shared_ptr<PipelineStageNode> tail;
    std::vector<std::future<std::any>> async_queue;
    std::mutex queue_mutex;
    ResultHandler result_handler;

};

