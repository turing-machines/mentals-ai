#pragma once

#include "core.h"
#include "buffer.h"
#include "chunker.h"
#include "file_manager.h"
#include "memory_controller.h"


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
        ///__memory_controller->write_chunks("books");
        return std::make_shared<int>(1);
    }

private:
    std::shared_ptr<MemoryController> __memory_controller;
    mutable std::mutex write_chunks_mutex;

};


class PipelineFactory {
public:
    template<typename Stage>
    void register_stage(const std::string& stage_name) {
        __creators[stage_name] = []() { return std::make_shared<Stage>(); };
    }

    template<typename Stage, typename... Args>
    void register_stage_with_args(const std::string& stage_name, Args... args) {
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


class Pipeline {
public:
    Pipeline(PipelineFactory& factory) : __factory(factory) {}

    Pipeline& stage(const std::string& stage_name) {
        auto stage = __factory.create_stage(stage_name);
        if (stage) { __stages.push_back({stage_name, stage}); } 
        else { fmt::print("Error: Unknown stage name {}\n", stage_name); }
        return *this;
    }

    using ResultHandler = std::function<void(const std::any&, std::any&)>;
    Pipeline& result_handler_async(ResultHandler handler) { 
        result_handler = handler;
        return *this;
    }

    std::any execute(const std::any& input) {
        std::any intermediate = input;
        for (const auto& [stage_name, stage] : __stages) {
            intermediate = stage->process(stage_name, intermediate);
            if (intermediate.has_value()) {
                if (intermediate.type() == typeid(std::shared_ptr<void>) 
                    && !std::any_cast<std::shared_ptr<void>>(intermediate)
                ) {
                    fmt::print("Pipeline stage '{}' failed. Stopping pipeline.\n", stage_name);
                    return std::any();
                }
            } else {
                fmt::print("Pipeline stage '{}' failed. Stopping pipeline.\n", stage_name);
                return std::any();
            }
        }
        return intermediate;
    }

    void execute_async(const std::any& input) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        auto future = std::async(std::launch::async, [this, input]() {
            auto result = this->execute(input);
            if (result_handler) { result_handler(input, result); }
            return result;
        });
        async_queue.emplace_back(std::move(future));
    }

private:
    PipelineFactory& __factory;
    std::vector<std::pair<std::string, std::shared_ptr<PipelineStageBase>>> __stages;
    std::vector<std::future<std::any>> async_queue;
    std::mutex queue_mutex;
    ResultHandler result_handler;

};

