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

};
