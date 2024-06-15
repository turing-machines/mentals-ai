#include "pgvector.h"


PgVector::PgVector(const std::string& conn_info) : conn_str(conn_info), conn(nullptr) {
    logger = Logger::get_instance();
}

PgVector::~PgVector() {}

tl::expected<void, std::string> PgVector::connect() {
    guard("PgVector::connect");
    conn = std::make_unique<pqxx::connection>(conn_str);
    if (!conn->is_open()) {
        logger->log("Failed to connect to database");
        return tl::unexpected<std::string>("Failed to connect to database");
    }
    logger->log(fmt::format("Connected to database: {}", conn->dbname()));
    unguard();
    return {};
}

tl::expected<json, std::string> PgVector::list_collections() {
    guard("PgVector::list_collections");
    if (!conn || !conn->is_open()) {
        return tl::unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    pqxx::result R = txn.exec("SELECT tablename FROM pg_tables WHERE schemaname = 'public';");
    json j_array = json::array();
    for (const auto& row : R) {
        j_array.push_back(row[0].c_str());
    }
    return j_array;
    unguard();
    return tl::unexpected<std::string>("Error when fetching the collection list");
}

tl::expected<std::string, std::string> PgVector::create_collection(const std::string& table_name, EmbeddingModel model) {
    guard("PgVector::create_collection");
    if (!conn || !conn->is_open()) {
        return tl::unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    pqxx::result r = txn.exec(fmt::format("SELECT EXISTS (SELECT FROM pg_tables WHERE tablename = '{}');", table_name));
    if (r[0][0].as<bool>()) {
        return tl::unexpected<std::string>(fmt::format("Table {} already exists", table_name));
    }
    txn.exec0("CREATE EXTENSION IF NOT EXISTS vector");
    int vector_dim = static_cast<int>(model);
    std::string sql = fmt::format(
        "CREATE TABLE IF NOT EXISTS {} (id bigserial PRIMARY KEY, content TEXT, embedding vector({}))",
        table_name, 
        vector_dim
    );
    txn.exec0(sql);
    txn.commit();
    unguard();
    return fmt::format("Table {} created successfully", table_name);
}

tl::expected<std::string, std::string> PgVector::delete_collection(const std::string& table_name) {
    guard("PgVector::delete_collection");
    if (!conn || !conn->is_open()) {
        return tl::unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    std::string sql = "DROP TABLE IF EXISTS " + table_name + ";";
    txn.exec(sql);
    txn.commit();
    unguard();
    return fmt::format("Table {} deleted successfully", table_name);
}

tl::expected<void, std::string> PgVector::write_content(const std::string& table_name, const std::string& content, const vdb::vector& embedding) {
    guard("PgVector::write_content");
    if (!conn || !conn->is_open()) {
        return tl::unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    std::string sql = fmt::format("INSERT INTO {} (content, embedding) VALUES ($1, $2)", table_name);
    txn.exec_params(sql, content, (std::ostringstream() << embedding).str());
    txn.commit();
    unguard();
    return {};
}

tl::expected<json, std::string> PgVector::search_content(const std::string& table_name, 
    const vdb::vector& search_vector, int limit) {
    guard("PgVector::search_content");
    if (!conn || !conn->is_open()) {
        return tl::unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    std::string sql = fmt::format(
        "SELECT *, embedding <=> $1 AS distance FROM {} ORDER BY distance LIMIT {}",
        //"SELECT * FROM {} ORDER BY embedding <=> $1 LIMIT {}",
        table_name, 
        limit
    );
    pqxx::result res = txn.exec_params(sql, (std::ostringstream() << search_vector).str());
    txn.commit();
    json j_results = json::array();
    for (auto row : res) {
        json j_item;
        j_item["content"] = row["content"].c_str();
        j_item["distance"] = row["distance"].as<double>();
        j_results.push_back(j_item);
    }
    return j_results;
    unguard();
    return tl::unexpected<std::string>("Nothing found");
}

