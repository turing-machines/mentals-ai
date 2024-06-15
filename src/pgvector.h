#pragma once

#include "core.h"

class PgVector {
private:
    std::string conn_str;
    std::unique_ptr<pqxx::connection> conn;

public:
    PgVector(const std::string &conn_info);
    ~PgVector();

    void connect();

    json list_collections();
    std::string create_collection(const std::string &table_name, EmbeddingModel model);
    std::string delete_collection(const std::string &table_name);
};
