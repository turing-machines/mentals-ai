#include "pgvector.h"


PgVector::PgVector(const std::string &conn_info) : conn_str(conn_info), conn(nullptr) {}

PgVector::~PgVector() {}

void PgVector::connect() {
    guard("PgVector::connect")
    conn = std::make_unique<pqxx::connection>(conn_str);
    if (conn->is_open()) {
        std::cout << "Connected to database: " << conn->dbname() << std::endl;
    } else {
        std::cout << "Failed to connect to database." << std::endl;
    }
    unguard()
}

json PgVector::list_collections() {
    guard("PgVector::list_collections")
    if (!conn || !conn->is_open()) {
        std::cout << "Connection is not open." << std::endl;
        return json::array();
    }
    pqxx::work txn(*conn);
    pqxx::result R = txn.exec("SELECT tablename FROM pg_tables WHERE schemaname = 'public';");
    json j_array = json::array();
    for (const auto& row : R) {
        j_array.push_back(row[0].c_str());
    }
    return j_array;
    unguard()
    return json::array();
}

std::string PgVector::create_collection(const std::string &table_name, EmbeddingModel model) {
    guard("PgVector::create_collection")
    if (!conn || !conn->is_open()) {
        return "Connection to vector db is not open";
    }
    pqxx::work txn(*conn);
    pqxx::result r = txn.exec(fmt::format("SELECT EXISTS (SELECT FROM pg_tables WHERE tablename = '{}');", table_name));
    if (r[0][0].as<bool>()) {
        return fmt::format("Table {} already exists", table_name);
    }
    txn.exec0("CREATE EXTENSION IF NOT EXISTS vector");
    int vector_dim = static_cast<int>(model);
    std::string sql = fmt::format(
        "CREATE TABLE IF NOT EXISTS {} (id SERIAL PRIMARY KEY, embedding vector({}));", 
        table_name, 
        vector_dim
    );
    txn.exec0(sql);
    txn.commit();
    return fmt::format("Table {} created successfully", table_name);
    unguard()
    return fmt::format("Table {} has not been created", table_name);
}

std::string PgVector::delete_collection(const std::string &table_name) {
    guard("PgVector::delete_collection")
    if (!conn || !conn->is_open()) {
        return "Connection is not open";
    }
    pqxx::work txn(*conn);
    std::string sql = "DROP TABLE IF EXISTS " + table_name + ";";
    txn.exec(sql);
    txn.commit();
    return fmt::format("Table {} deleted successfully", table_name);
    unguard()
    return fmt::format("Table {} has not been deleted", table_name); 
}
