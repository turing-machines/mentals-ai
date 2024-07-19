#pragma once

#include "core.h"

class Chunker {
public:
    virtual expected<std::vector<std::string>, std::string> process(const std::string& text) const = 0;
    virtual ~Chunker() = default;
};

class NaiveChunker : public Chunker {
private:
    int __sentences_per_chunk;

public:
    NaiveChunker(int sentences_per_chunk) : __sentences_per_chunk(sentences_per_chunk) {}

    /// TODO: Overload StringBuffer->SafeChunkBuffer

    expected<std::vector<std::string>, std::string> process(const std::string& text) const override {
        guard("NaiveChunker::process")
        std::vector<std::string> chunks;
        std::istringstream stream(text);
        std::string sentence;
        std::string chunk;
        int count = 0;
        while (std::getline(stream, sentence, '.')) {
            sentence.erase(sentence.begin(), 
                std::find_if(sentence.begin(), sentence.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
            if (!sentence.empty()) {
                chunk += sentence + ".";
                count++;
            }
            if (count == __sentences_per_chunk) {
                chunks.push_back(chunk);
                chunk = "";
                count = 0;
            }
        }
        if (!chunk.empty()) {
            chunks.push_back(chunk);
        }
        return chunks;
        unguard()
        return unexpected("Error during chunking");
    }

    expected<std::shared_ptr<SafeChunkBuffer<std::string>>, std::string> 
    process(const StringBuffer& input_buffer) const {
        guard("NaiveChunker::process(StringBuffer)")
        std::string text = input_buffer.get_data();
        auto result = process(text);
        if (result.has_value()) {
            auto chunk_buffer = std::make_shared<SafeChunkBuffer<std::string>>();
            /// TODO: Overload set_data to handle std::vector<T>
            ///chunk_buffer->set_data(result.value());
            return chunk_buffer;
        } else {
            return unexpected(result.error());
        }
        unguard()
        return unexpected("Error during chunking");
    }

};
