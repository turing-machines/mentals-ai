#pragma once

#include "core.h"
#include "logger.h"

class PgVector {
private:
    std::string conn_str;
    std::unique_ptr<pqxx::connection> conn;

    Logger* logger;

public:
    PgVector(const std::string &conn_info);
    ~PgVector();

    tl::expected<void, std::string> connect();
    tl::expected<json, std::string> list_collections();
    tl::expected<std::string, std::string> create_collection(const std::string &table_name, EmbeddingModel model);
    tl::expected<std::string, std::string> delete_collection(const std::string &table_name);
    tl::expected<void, std::string> write_content(const std::string &table_name, const std::string &content, 
        const vdb::Vector &embedding);
};
