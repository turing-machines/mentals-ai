#pragma once

#include "core.h"
#include "llm.h"

struct MemChunk {
    int chunk_id;
    std::string idx;
    std::string content;
    vdb::vector embedding;
    std::optional<std::string> name;
    std::optional<std::string> meta;
};

MemChunk process_chunks_async(
    LLM& llm, EmbeddingModel embed_model,
    int chunk_id, const std::string& idx,
    const std::string& content,
    const std::optional<std::string>& name = std::nullopt,
    const std::optional<std::string>& meta = std::nullopt) {
    fmt::print("\033[0mmem_chunk #{}: Embedding started.\n", chunk_id);
    std::future<vdb::vector> future_vec = llm.embedding_async(content, embed_model);
    vdb::vector embedding = future_vec.get();
    fmt::print("\033[0mmem_chunk #{}: Embedding completed.\n", chunk_id);
    return { chunk_id, idx, content, embedding, name, meta };
}

/// Naive method just for testing purposes
/// TODO: Different strategies e.g. w/ pages, paragraph, overlapping etc. 
std::vector<std::string> split_text_by_sentences(const std::string& text, int sentences_per_chunk) {
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
        if (count == sentences_per_chunk) {
            chunks.push_back(chunk);
            chunk = "";
            count = 0;
        }
    }
    if (!chunk.empty()) {
        chunks.push_back(chunk);
    }
    return chunks;
}


