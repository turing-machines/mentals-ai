#pragma once

#include "core.h"
#include "logger.h"

/*
<-> - L2 distance
<#> - (negative) inner product
<=> - cosine distance
<+> - L1 distance (added in 0.7.0)
*/

using tl::expected, tl::unexpected;

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
        {vdb::QueryType::embedding, {"SELECT * FROM {{table_name}} ORDER BY embedding {{distance_function}} $1 LIMIT {{limit}}", std::nullopt}},
        {vdb::QueryType::distance, {"SELECT *, embedding {{distance_function}} $1 AS distance FROM {{table_name}} ORDER BY distance LIMIT {{limit}}", "distance"}},
        {vdb::QueryType::cosine_similarity, {"SELECT *, 1 - (embedding {{distance_function}} $1) AS cosine_similarity FROM {{table_name}} ORDER BY cosine_similarity DESC LIMIT {{limit}}", "cosine_similarity"}}
    };

public:
    PgVector(const std::string& conn_info);
    ~PgVector();

    expected<void, std::string> connect();
    expected<json, std::string> list_collections();
    expected<std::string, std::string> create_collection(const std::string& table_name, EmbeddingModel model);
    expected<std::string, std::string> delete_collection(const std::string& table_name);
    expected<void, std::string> write_content(const std::string& table_name, const std::string& content, 
        const vdb::vector& embedding);
    expected<json, std::string> search_content(const std::string& table_name, const vdb::vector& search_vector, 
        int limit, vdb::QueryType type = vdb::QueryType::embedding);
};
