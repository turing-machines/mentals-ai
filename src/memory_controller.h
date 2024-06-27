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

    embedding_model embed_model;
    int processed_tokens;

private:
    expected<mem_chunk*, std::string> chunk_embedding(
        const int chunk_id,
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
        embed_model = embedding_model::oai_3small;
        processed_tokens = 0;
    }

    embedding_model get_model() const { return embed_model; }
    void set_model(embedding_model new_embed_model) { embed_model = new_embed_model; }

    void create_collection(const std::string& collection) {
        __vdb.create_collection(collection, embed_model);
    }
    void delete_collection(const std::string& collection) {
        __vdb.delete_collection(collection);
    }

    void process_chunks( 
        const std::vector<std::string>& chunks,
        const std::optional<std::string>& name = std::nullopt,
        const std::optional<std::string>& meta = std::nullopt
    ) {
        std::string idx = gen_index();
        for (std::size_t i = 0; i < chunks.size(); ++i) {
            std::string chunk_content = chunks[i];
            fmt::print("\033[0mmem_chunk #{}: Processing started.\n", i);
            futures.push_back(std::async(std::launch::async, 
                &MemoryController::chunk_embedding, this, 
                i, idx, chunk_content, name, meta
            ));
        }
    }

    expected<void, std::vector<int>> write_chunks(const std::string& collection) {
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
            __vdb.write_content(*txn, collection, chunk->idx, chunk->content, chunk->embedding, chunk->name, chunk->meta);
            fmt::print("\033[0mmem_chunk #{}: Data written to memory.\n", chunk->chunk_id);
        }
        __vdb.commit_transaction(txn);
        fmt::print("\n{} chunks have been processed\n{} processed tokens\n\n", futures.size(), processed_tokens);
        futures.clear();
        mem_chunks.clear();
        processed_tokens = 0;
        if (!failed_chunks.empty()) {
            /// TODO: Unroll transaction or try to write in the next call
            return unexpected(failed_chunks);
        }
        unguard()
        return {};
    }

    expected<json, std::string> read_chunks(
        const std::string& collection, 
        const std::string& query,
        const int count
    ) {
        guard("MemoryController::read_chunks")
        fmt::print("Fetch query: {}\n\n", query);
        liboai::Response response = __llm.embedding(query, embed_model);
        json jres = response["data"][0]["embedding"];
        vdb::vector query_vector = vdb::vector({ jres.begin(), jres.end() }, embed_model);
        auto result = __vdb.search_content(collection, query_vector, count, 
            vdb::query_type::cosine_similarity);
        return result;
        unguard()
        return unexpected("");
    }

};
