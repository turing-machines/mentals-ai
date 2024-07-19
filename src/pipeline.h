#pragma once

#include "core.h"
#include "buffer.h"
#include "chunker.h"
#include "file_manager.h"


template <typename In, typename Out>
class PipelineStage {
public:
    PipelineStage() = default;
    virtual ~PipelineStage() = default;

    virtual std::shared_ptr<Out> process(const std::shared_ptr<In>& input) = 0;

};


class FileToStringBuffer : public PipelineStage<std::string, StringBuffer> {
public:
    FileToStringBuffer(const std::shared_ptr<FileManager>& file_manager)
        : __file_manager(file_manager) {}

    std::shared_ptr<StringBuffer> process(const std::shared_ptr<std::string>& file_path) override {
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

    std::shared_ptr<SafeChunkBuffer<T>> process(const std::shared_ptr<StringBuffer>& string_buffer) override {
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

    std::shared_ptr<void> create_stage(const std::string& stage_name) {
        auto it = __creators.find(stage_name);
        if (it != __creators.end()) { return it->second(); }
        return nullptr;
    }

private:
    std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> __creators;

};


template <typename In, typename Out>
class Pipeline {
public:
    using ResultHandler = std::function<void(const In&, std::shared_ptr<Out>)>;
    void async_result_handler(ResultHandler handler) { result_handler = handler; }

    Pipeline(PipelineFactory& factory) : __factory(factory) {}

    void add_stage(const std::string& stage_name) {
        guard("Pipeline::add_stage")
        auto stage = std::static_pointer_cast<PipelineStage<In, Out>>(__factory.create_stage(stage_name));
        if (stage) { __stages.emplace_back(stage_name, stage);
        } else { fmt::print("Error: Unknown stage name {}\n", stage_name); }
        unguard()
    }

    std::shared_ptr<Out> lfg(const In& input) {
        guard("Pipeline::lfg")
        std::shared_ptr<void> intermediate = std::make_shared<In>(input);
        for (const auto& [stage_name, stage] : __stages) {
            intermediate = stage->process(std::static_pointer_cast<In>(intermediate));
            if (!intermediate) {
                fmt::print("Pipeline stage '{}' failed. Stopping pipeline.\n", stage_name);
                return nullptr;
            }
        }
        return std::static_pointer_cast<Out>(intermediate);
        unguard()
        return nullptr;
    }

    void lfg_async(const In& input) {
        guard("Pipeline::lfg_async")
        std::lock_guard<std::mutex> lock(queue_mutex);
        auto future = std::async(std::launch::async, [this, input]() {
            auto result = this->lfg(input);
            if (result_handler) { result_handler(input, result); }
            return result;
        });
        async_queue.emplace_back(std::move(future));
        unguard()
    }

private:
    PipelineFactory& __factory;
    std::vector<std::pair<std::string, std::shared_ptr<PipelineStage<In, Out>>>> __stages;
    std::vector<std::future<std::shared_ptr<Out>>> async_queue;
    std::mutex queue_mutex;
    ResultHandler result_handler;

};
