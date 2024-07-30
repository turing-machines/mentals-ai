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
    std::mutex mem_chunks_mutex;

    embedding_model embed_model;

    /// Stats
    double processed_kb;
    int processed_tokens;

private:
    expected<mem_chunk*, std::string> chunk_embedding(
        const std::string& content_id,
        const int chunk_id,
        const std::string& content,
        const std::optional<std::string>& name = std::nullopt,
        const std::optional<std::string>& meta = std::nullopt
    ) {
        guard("MemoryController::chunk_embedding")
        fmt::print("\033[0mmem_chunk #{}#{}: Embedding started.\n", content_id, chunk_id);
        std::string cleaned_content = content;
        if (!is_valid_utf8(content)) {
            fmt::print("\033[31mError: Invalid UTF-8 byte in content for chunk #{}#{}.\n", content_id, chunk_id);
            cleaned_content = remove_invalid_utf8(content);
        }
        size_t size_in_bytes = cleaned_content.size();
        double size_in_kb = static_cast<double>(size_in_bytes) / 1024.0;
        processed_kb += size_in_kb;
        std::future<liboai::Response> future_res = __llm.embedding_async(cleaned_content, embed_model);
        liboai::Response response = future_res.get();
        processed_tokens += response["usage"]["total_tokens"].get<int>();
        json jres = response["data"][0]["embedding"];
        vdb::vector embedding({ jres.begin(), jres.end() }, embed_model);
        fmt::print("\033[0mmem_chunk #{}#{}: Embedding completed.\n", content_id, chunk_id);
        ///std::lock_guard<std::mutex> lock(mem_chunks_mutex);
        mem_chunks.emplace_back(mem_chunk{
            content_id, chunk_id, cleaned_content, embedding, name, meta
        });
        return &mem_chunks.back();
        unguard()
        return {};
    }

public:
    MemoryController(LLM& llm, PgVector& vdb) : __llm(llm), __vdb(vdb) {
        embed_model = embedding_model::oai_3small;
        processed_kb = 0;
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
        std::string content_id = gen_index();
        for (size_t chunk_id = 0; chunk_id < chunks.size(); ++chunk_id) {
            std::string chunk_content = chunks[chunk_id];
            fmt::print("\033[0mmem_chunk #{}#{}: Processing started.\n", content_id, chunk_id);
            futures.push_back(std::async(std::launch::async, 
                &MemoryController::chunk_embedding, this, 
                content_id, chunk_id, chunk_content, name, meta
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
                fmt::print("\033[31mError processing chunk {}\n", res.error());
                failed_chunks.push_back(fut.get().value()->chunk_id);
                continue;
            }
            mem_chunk* chunk = res.value();
            __vdb.write_content(*txn, collection, *chunk);
            fmt::print("\033[0mmem_chunk #{}#{}: Data written to memory.\n", 
                chunk->content_id, chunk->chunk_id);
        }
        __vdb.commit_transaction(txn);
        fmt::print("\n{} chunks have been processed\n{} failed chunks\n"
            "{} processed tokens\n{:.2f} MB of data processed\n",
            mem_chunks.size(), failed_chunks.size(), processed_tokens, processed_kb / 1024.0);
        futures.clear();
        {
            ///std::lock_guard<std::mutex> lock(mem_chunks_mutex);
            mem_chunks.clear();
        }
        processed_kb = 0;
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
