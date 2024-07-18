#pragma once

#include "core.h"
#include "buffer.h"
#include "pgvector.h"
#include "embeddings.h"

class MemoryController {
private:
    std::shared_ptr<EmbeddingsInterface> __emb;
    std::shared_ptr<PgVector> __vdb;  /// TODO: MemoryInterface

    std::vector<std::future<expected<mem_chunk*, std::string>>> futures;
    SafeChunkBuffer<mem_chunk> chunk_buffer;

    /// Stats
    double processed_kb;
    int processed_tokens;
    size_t total_chunks;
    size_t processed_chunks;
    size_t written_chunks;

public:
    using ProgressCallback = std::function<void(double)>;

private:
    ProgressCallback progress_callback;

    void update_progress() {
        if (progress_callback && total_chunks > 0) {
            double progress = static_cast<double>(processed_chunks + written_chunks) / (2 * total_chunks) * 100.0;
            progress_callback(progress);
        }
    }

    expected<mem_chunk*, std::string> chunk_embeddings(mem_chunk& chunk) {
        guard("MemoryController::chunk_embeddings")
        std::string cleaned_content = chunk.content;
        if (!is_valid_utf8(chunk.content)) {
            fmt::print("{}Warning: Invalid UTF-8 byte in content for chunk #{}#{}. Cleaning..{}\n", MAGENTA, chunk.content_id, chunk.chunk_id, RESET);
            cleaned_content = remove_invalid_utf8(chunk.content);
        }
        std::future<liboai::Response> future_res = __emb->embeddings_async(cleaned_content);
        liboai::Response response = future_res.get();
        processed_tokens += response["usage"]["total_tokens"].get<int>();
        json jres = response["data"][0]["embedding"];
        vdb::vector embedding({ jres.begin(), jres.end() }, __emb->get_model());
        chunk.embedding = embedding;
        chunk.content = cleaned_content;
        chunk_buffer.push_back(chunk);
        size_t size_in_bytes = cleaned_content.size();
        double size_in_kb = static_cast<double>(size_in_bytes) / 1024.0;
        processed_kb += size_in_kb;
        processed_chunks++;
        update_progress();
        return &chunk_buffer.back();
        unguard()
        return {};
    }

public:
    expected<mem_chunk*, std::string> buf_get_chunk(const std::string& content) {
        guard("MemoryController::buf_get_chunk")
        auto chunk_result = chunk_buffer.get_chunk([&content](const mem_chunk& chunk) {
            return chunk.content == content;
        });
        if (chunk_result.has_value()) {
            return *chunk_result;
        }
        unguard()
        return unexpected("mem_chunk not found");
    }

    void buf_clear() { chunk_buffer.clear(); }

public:

    MemoryController(std::shared_ptr<EmbeddingsInterface> emb, std::shared_ptr<PgVector> vdb)
        : __emb(std::move(emb)), __vdb(std::move(vdb)), processed_kb(0), processed_tokens(0)
        , total_chunks(0), processed_chunks(0), written_chunks(0) {}

    ~MemoryController() = default;

    embedding_model get_model() const { return __emb->get_model(); }
    void set_model(embedding_model new_embed_model) { __emb->set_model(new_embed_model); }

    void set_progress_callback(ProgressCallback callback) { progress_callback = callback; }

    /// TODO: Deprecate from this class
    void create_collection(const std::string& collection) {
        __vdb->create_collection(collection, __emb->get_model());
    }
    void delete_collection(const std::string& collection) {
        __vdb->delete_collection(collection);
    }
    ///

    void process_chunks( 
        const std::vector<std::string>& chunks,
        const std::optional<std::string>& name = std::nullopt,
        const std::optional<std::string>& meta = std::nullopt
    ) {
        total_chunks += chunks.size();
        update_progress();
        std::string content_id;
        if (name && !name->empty()) { content_id = gen_index(*name); }
            else { content_id = gen_index(); }
        for (size_t chunk_id = 0; chunk_id < chunks.size(); ++chunk_id) {
            std::string chunk_content = chunks[chunk_id];
            mem_chunk chunk { content_id, static_cast<int>(chunk_id), chunk_content, name, meta };
            ///fmt::print("{}mem_chunk #{}#{}: Processing started.\n", RESET, content_id, chunk_id);
            futures.push_back(std::async(std::launch::async, 
                &MemoryController::chunk_embeddings, this, std::ref(chunk)
            ));
        }
    }

    expected<void, std::vector<int>> write_chunks(const std::string& collection, const bool& clear_buffer = true) {
        std::vector<int> failed_chunks;
        guard("MemoryController::write_chunks")
        auto txn = __vdb->create_transaction();
        for (auto& fut : futures) {
            auto res = fut.get();
            if (!res.has_value()) {
                ///fmt::print("{}Error processing chunk {}.{}\n", RED, res.error(), RESET);
                failed_chunks.push_back(fut.get().value()->chunk_id);
                continue;
            }
            mem_chunk* chunk = res.value();
            __vdb->write_content(*txn, collection, *chunk);
            ///fmt::print("{}mem_chunk #{}#{}: Data written to memory.\n", RESET, 
            ///    chunk->content_id, chunk->chunk_id);
            written_chunks++;
            update_progress();
        }
        __vdb->commit_transaction(txn);
        if (progress_callback) { progress_callback(100.0); }
        fmt::print(
            "\n----------------\n"
            "{} chunks have been processed\n"
            "{} failed chunks\n"
            "{} processed tokens\n"
            "{:.2f} MB of data processed\n",
            chunk_buffer.size(), failed_chunks.size(), processed_tokens, processed_kb / 1024.0);
        futures.clear();
        if (clear_buffer) { chunk_buffer.clear(); }
        processed_kb = 0;
        processed_tokens = 0;
        total_chunks = 0;
        processed_chunks = 0;
        written_chunks = 0;
        if (!failed_chunks.empty()) {
            /// TODO: Unroll transaction or try to write in the next call
            return unexpected(failed_chunks);
        }
        unguard()
        return {};
    }

    expected<std::vector<mem_chunk>, std::string> read_chunks(
        const std::string& collection,
        const std::optional<std::string>& content_id = std::nullopt,
        const std::optional<int>& num_chunks = std::nullopt
    ) {
        guard("MemoryController::read_chunks")
        auto result = __vdb->read_content(collection, content_id, num_chunks);
        return result;
        unguard()
        return unexpected("");
    }

    expected<std::vector<mem_chunk>, std::string> read_chunks(
        const std::string& collection,
        const std::optional<std::string>& content_id = std::nullopt,
        const std::optional<int>& num_chunks = std::nullopt,
        const std::optional<std::string>& start_time = std::nullopt,
        const std::optional<bool>& before = std::nullopt
    ) {
        guard("MemoryController::read_chunks")
        auto result = __vdb->read_content(collection, content_id, num_chunks, start_time, before);
        return result;
        unguard()
        return unexpected("");
    }

    expected<std::vector<mem_chunk>, std::string> read_chunks(
        const std::string& collection, 
        const std::string& query,
        const int& num_chunks,
        vdb::query_type query_type = vdb::query_type::cosine_similarity
    ) {
        guard("MemoryController::read_chunks[query]")
        fmt::print("Fetch query: {}\n\n", query);
        vdb::vector query_vector;
        auto buffer_result = buf_get_chunk(query);
        if (buffer_result.has_value()) {
            ///fmt::print("Embedding vector found in buffer.\n");
            query_vector = buffer_result.value()->embedding;
        } else {
            /// TODO: Add response validator
            liboai::Response response = __emb->embeddings(query);
            json jres = response["data"][0]["embedding"];
            query_vector = vdb::vector({ jres.begin(), jres.end() }, __emb->get_model());
        }
        auto result = __vdb->search_content(collection, query_vector, num_chunks, query_type);
        ///auto result = __vdb->bm25_search_content(collection, query, num_chunks);
        return result;
        unguard()
        return unexpected("");
    }

};
