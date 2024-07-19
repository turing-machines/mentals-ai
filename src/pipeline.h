#pragma once

#include "core.h"
#include "buffer.h"
#include "chunker.h"
#include "file_manager.h"


class PipelineStageBase {
public:
    virtual ~PipelineStageBase() = default;
    virtual void process() = 0;

};

template <typename In, typename Out>
class PipelineStage : public PipelineStageBase {
public:
    PipelineStage(std::shared_ptr<In> input, std::shared_ptr<Out> output)
        : __input(input), __output(output) {}

protected:
    std::shared_ptr<In> __input;
    std::shared_ptr<Out> __output;

};

/// TODO: FileManager -> FileReader
class FileToStringBuffer : public PipelineStage<FileManager, StringBuffer> {
public:
    FileToStringBuffer(
        const std::string& file_path, 
        std::shared_ptr<FileManager> file_manager, 
        std::shared_ptr<StringBuffer> string_buffer)
        : PipelineStage(file_manager, string_buffer), __file_path(file_path) {}

    void process() override {
        auto file_content = __input->read_file(__file_path);
        if (file_content.has_value()) {
            __output->set_data(file_content.value());
        } else {
            fmt::print("Error reading file: {}\n", file_content.error());
        }
    }

private:
    std::string __file_path;

};


template <typename T>
class StringBufferToChunkBuffer : public PipelineStage<StringBuffer, SafeChunkBuffer<T>> {
public:
    StringBufferToChunkBuffer(
        std::shared_ptr<StringBuffer> string_buffer,
        std::shared_ptr<SafeChunkBuffer<T>> chunk_buffer, 
        std::shared_ptr<Chunker> chunker = nullptr)
        : PipelineStage(string_buffer, chunk_buffer), __chunker(chunker) {
            if (!__chunker) {
                __chunker = std::make_unique<NaiveChunker>(SENTENCES_PER_CHUNK);
            }
        }

    void process() override {
        std::string data = __input->get_data();
        auto result = __chunker->process(data);
        if (result.has_value()) {
            /// TODO: Overload set_data
            ///__output->set_data(result.value());
        } else {
            fmt::print("Error during chunking: {}\n", result.error());
        }
    }

private:
    std::shared_ptr<Chunker> __chunker;

};

