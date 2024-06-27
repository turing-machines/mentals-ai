#pragma once

#include "core.h"
#include "pgvector.h"
#include "llm.h"

class MemoryController {
private:
    LLM& __llm;
    PgVector& __vdb; /// TODO: MemoryInterface

    std::vector<std::future<expected<mem_chunk*, std::string>>> futures;
    std::deque<mem_chunk> mem_chunks;

    int processed_tokens;

private:
    expected<mem_chunk*, std::string> chunk_embedding(
        embedding_model embed_model,
        int chunk_id,
        const std::string& idx,
        const std::string& content,
        const std::optional<std::string>& name = std::nullopt,
        const std::optional<std::string>& meta = std::nullopt
    ) {
        guard("MemoryController::chunk_embedding")
        fmt::print("\033[0mmem_chunk #{}: Embedding started.\n", chunk_id);
        std::future<liboai::Response> future_res = __llm.embedding_async(content, embed_model);
        liboai::Response response = future_res.get();
        processed_tokens += response["usage"]["total_tokens"].get<int>();
        json jres = response["data"][0]["embedding"];
        vdb::vector embedding({ jres.begin(), jres.end() }, embed_model);
        fmt::print("\033[0mmem_chunk #{}: Embedding completed.\n", chunk_id);
        mem_chunks.emplace_back(mem_chunk{
            chunk_id, idx, content, embedding, name, meta
        });
        return &mem_chunks.back();
        unguard()
        return {};
    }

public:
    MemoryController(LLM& llm, PgVector& vdb) : __llm(llm), __vdb(vdb) {
        processed_tokens = 0;
    }

    void process_chunks(embedding_model embed_model, 
        const std::vector<std::string>& chunks,
        const std::string& idx,
        const std::optional<std::string>& name = std::nullopt,
        const std::optional<std::string>& meta = std::nullopt
    ) {
        for (std::size_t i = 0; i < chunks.size(); ++i) {
            std::string chunk_content = chunks[i];
            fmt::print("\033[0mmem_chunk #{}: Processing started.\n", i);
            futures.push_back(std::async(std::launch::async, 
                &MemoryController::chunk_embedding, this, 
                embed_model, i, idx, chunk_content, name, meta
            ));
        }
    }

    expected<void, std::vector<int>> write_chunks(const std::string& partition) {
        std::vector<int> failed_chunks;
        guard("MemoryController::write_chunks")
        auto txn = __vdb.create_transaction();
        for (auto& fut : futures) {
            auto res = fut.get();
            if (!res.has_value()) {
                fmt::print("Error processing chunk: {}\n", res.error());
                failed_chunks.push_back(fut.get().value()->chunk_id);
                continue;
            }
            mem_chunk* chunk = res.value();
            /// TODO: Overload with mem_chunk
            __vdb.write_content(*txn, partition, chunk->idx, chunk->content, chunk->embedding, chunk->name, chunk->meta);
            fmt::print("\033[0mmem_chunk #{}: Data written to memory.\n", chunk->chunk_id);
        }
        __vdb.commit_transaction(txn);
        fmt::print("\n{} chunks have been processed\n{} processed tokens\n\n", futures.size(), processed_tokens);
        futures.clear();
        mem_chunks.clear();
        processed_tokens = 0;
        if (!failed_chunks.empty()) {
            /// TODO: Unroll transaction or try to write in the next call
            return unexpected<std::vector<int>>(failed_chunks);
        }
        unguard()
        return {};
    }

    json read_chunks(const std::string& partition, const std::string& query, embedding_model embed_model) {
        guard("MemoryController::read_chunks")
        fmt::print("Fetch query: {}\n\n", query);
        liboai::Response response = __llm.embedding(query, embed_model);
        json jres = response["data"][0]["embedding"];
        vdb::vector query_vector = vdb::vector({ jres.begin(), jres.end() }, embed_model);
        auto result = __vdb.search_content(partition, query_vector, 20, 
            vdb::query_type::cosine_similarity);
        return result.value();
        unguard()
        return json({});
    }

};
