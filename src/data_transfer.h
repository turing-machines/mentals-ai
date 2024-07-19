#pragma once

#include "core.h"
#include "chunker.h"
#include "file_manager.h"
#include "memory_controller.h"


/// TODO: implement pipeline pattern
template <typename Source, typename Target>
class DataTransfer {
private:
    std::shared_ptr<Source> __source;
    std::shared_ptr<Target> __target;
    std::unique_ptr<Chunker> chunker;

public:
    DataTransfer(std::shared_ptr<Source> source, std::shared_ptr<Target> target, std::unique_ptr<Chunker> chunker = nullptr)
        : __source(source), __target(target), chunker(std::move(chunker)) {
        if (!chunker) {
            chunker = std::make_unique<NaiveChunker>(SENTENCES_PER_CHUNK);
        }
    }

    void set_strategy(std::unique_ptr<Chunker> chunker) {
        chunker = std::move(chunker);
    }

    void transfer(const std::string& source_address, const std::string& target_address) {
        /// TODO: Abstract this part
        /// Get data from source
        auto content = __source->read_file(source_address);
        if (content.has_value()) {
            auto chunks = chunker->process(*content);
            if (chunks.has_value()) {
                __target->process_chunks(*chunks, fs::path(source_address).filename().string());
                auto result = __target->write_chunks(target_address);
                if (!result.has_value()) {
                    for (int failed_chunk : result.error()) {
                        /// TODO: Handle failed chunks
                    }
                }
            } else {
                /// TODO: Try chunking again 
            }
        }
    }

    void bulk_transfer(const std::string& source_address, const std::string& target_address) {
        /// TODO: Abstract this part
        std::vector<std::future<void>> futures;
        for (auto it = __source->begin(source_address); it != __source->end(); ++it) {
            const auto& entry = *it;
            if (entry.is_regular_file()) {
                std::string source_address = entry.path().string();
                if (DocFactory::is_supported(source_address)) {
                    futures.push_back(std::async(std::launch::async, [this, source_address, target_address, entry] {
                        auto content = __source->read_file(source_address);
                        if (content.has_value()) {
                            auto chunks = chunker->process(*content);
                            if (chunks.has_value()) {
                                __target->process_chunks(*chunks, entry.path().filename().string());
                            }
                        }
                    }));
                }
            }
        }
        for (auto& future : futures) { future.get(); }
        auto result = __target->write_chunks(target_address);
        if (!result.has_value()) {
            for (int failed_chunk : result.error()) {
                /// TODO: Handle failed chunks
            }
        }
    }

    void transfer_content(const std::string& target_address, const std::string& content_id, const std::string& source_address) {
        auto chunks = __target->read_chunks(target_address, content_id, 10); /// For testing purposes
        if (chunks.has_value()) {
            std::string content;
            for (const auto& chunk : chunks.value()) {
                content += chunk.content;
            }
            __source->write_file(source_address, content);
        } else {
            /// TODO: Handle read chunks error
        }
    }

};
