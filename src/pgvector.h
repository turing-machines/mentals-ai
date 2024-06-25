#pragma once

#include "core.h"
#include "logger.h"

#include <pqxx/pqxx>

/*
<-> - L2 distance
<#> - (negative) inner product
<=> - cosine distance
<+> - L1 distance (added in 0.7.0)
*/

#define QUERY_EMBEDDING \
    "SELECT * FROM {{table_name}} ORDER BY embedding {{distance_function}} $1 LIMIT {{limit}}"

#define QUERY_DISTANCE \
    "SELECT *, embedding {{distance_function}} $1 AS distance " \
    "FROM {{table_name}} ORDER BY distance LIMIT {{limit}}"

#define QUERY_COSINE_SIMILARITY \
    "SELECT *, 1 - (embedding {{distance_function}} $1) AS cosine_similarity " \
    "FROM {{table_name}} ORDER BY cosine_similarity DESC LIMIT {{limit}}"

const std::string L1Distance = "<+>";
const std::string L2Distance = "<->";
const std::string NegativeInnerProduct = "<#>";
const std::string CosineDistance = "<=>";

class PgVector {
private:
    std::string conn_str;
    std::unique_ptr<pqxx::connection> conn;

    Logger* logger;

private:
    std::map<vdb::QueryType, vdb::QueryInfo> query_options = {
        { vdb::QueryType::embedding         , { QUERY_EMBEDDING         , std::nullopt          }},
        { vdb::QueryType::distance          , { QUERY_DISTANCE          , "distance"            }},
        { vdb::QueryType::cosine_similarity , { QUERY_COSINE_SIMILARITY , "cosine_similarity"   }}
     };

public:
    PgVector(const std::string& conn_info);
    ~PgVector();

    expected<void, std::string> connect();
    expected<json, std::string> list_collections();
    expected<json, std::string> create_collection(const std::string& table_name, 
        EmbeddingModel model = EmbeddingModel::oai_3small);
    expected<std::string, std::string> delete_collection(const std::string& table_name);
    expected<json, std::string> get_collection_info(const std::string& table_name);
    expected<void, std::string> write_content(
        const std::string& table_name,
        const std::string& content_id, 
        const std::string& content, 
        const vdb::vector& embedding, 
        const std::optional<std::string>& name = std::nullopt, 
        const std::optional<std::string>& desc = std::nullopt);
    expected<json, std::string> search_content(const std::string& table_name, const vdb::vector& search_vector, 
        int limit, vdb::QueryType type = vdb::QueryType::embedding);
};
