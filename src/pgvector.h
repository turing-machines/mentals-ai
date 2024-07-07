#pragma once

#include "core.h"
#include "logger.h"

#include "sql_builder.h"

#include <pqxx/pqxx>

/*
<-> - L2 distance
<#> - (negative) inner product
<=> - cosine distance
<+> - L1 distance (added in 0.7.0)
*/

const std::string L1Distance = "<+>";
const std::string L2Distance = "<->";
const std::string NegativeInnerProduct = "<#>";
const std::string CosineDistance = "<=>";

class PgVector {
private:
    std::string conn_str;
    std::unique_ptr<pqxx::connection> conn;

    Logger* logger;

/*private:
    std::map<vdb::query_type, vdb::query_info> query_options = {
        { vdb::query_type::embedding         , { QUERY_EMBEDDING         , std::nullopt          }},
        { vdb::query_type::distance          , { QUERY_DISTANCE          , "distance"            }},
        { vdb::query_type::cosine_similarity , { QUERY_COSINE_SIMILARITY , "cosine_similarity"   }}
     };
*/
public:
    PgVector(const std::string& conn_info);
    ~PgVector();

    expected<void, std::string> connect();
    std::unique_ptr<pqxx::work> create_transaction();
    void commit_transaction(std::unique_ptr<pqxx::work>& txn);
    expected<json, std::string> list_collections();
    expected<json, std::string> create_collection(const std::string& table_name, 
        embedding_model model = embedding_model::oai_3small);
    expected<std::string, std::string> delete_collection(const std::string& table_name);
    expected<json, std::string> get_collection_info(const std::string& table_name);
    expected<void, std::string> write_content(
        pqxx::work& txn,
        const std::string& table_name,
        const std::string& content_id, 
        const int chunk_id,
        const std::string& content, 
        const vdb::vector& embedding, 
        const std::optional<std::string>& name = std::nullopt, 
        const std::optional<std::string>& desc = std::nullopt);
    expected<void, std::string> write_content(
        pqxx::work& txn,
        const std::string& table_name,
        const mem_chunk& chunk);
    expected<std::vector<mem_chunk>, std::string> read_content(
        const std::string& table_name,
        const std::optional<std::string>& content_id = std::nullopt,
        const std::optional<int>& num_chunks = std::nullopt);
    expected<std::vector<mem_chunk>, std::string> search_content(
        const std::string& table_name, 
        const vdb::vector& search_vector, 
        int limit, 
        vdb::query_type type = vdb::query_type::embedding);
};
