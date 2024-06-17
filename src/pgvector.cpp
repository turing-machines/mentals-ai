#include "pgvector.h"


PgVector::PgVector(const std::string& conn_info) : conn_str(conn_info), conn(nullptr) {
    logger = Logger::get_instance();
}

PgVector::~PgVector() {}

expected<void, std::string> PgVector::connect() {
    guard("PgVector::connect");
    conn = std::make_unique<pqxx::connection>(conn_str);
    if (!conn->is_open()) {
        logger->log("Failed to connect to database");
        return unexpected<std::string>("Failed to connect to database");
    }
    logger->log(fmt::format("Connected to database: {}", conn->dbname()));
    unguard();
    return {};
}

expected<json, std::string> PgVector::list_collections() {
    guard("PgVector::list_collections");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    pqxx::result R = txn.exec("SELECT tablename FROM pg_tables WHERE schemaname = 'public';");
    json j_array = json::array();
    for (const auto& row : R) {
        j_array.push_back(row[0].c_str());
    }
    return j_array;
    unguard();
    return unexpected<std::string>("Error when fetching the collection list");
}

expected<std::string, std::string> PgVector::create_collection(const std::string& table_name, EmbeddingModel model) {
    guard("PgVector::create_collection");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    pqxx::result r = txn.exec(fmt::format("SELECT EXISTS (SELECT FROM pg_tables WHERE tablename = '{}');", table_name));
    if (r[0][0].as<bool>()) {
        return unexpected<std::string>(fmt::format("Table {} already exists", table_name));
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

expected<std::string, std::string> PgVector::delete_collection(const std::string& table_name) {
    guard("PgVector::delete_collection");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    std::string sql = "DROP TABLE IF EXISTS " + table_name + ";";
    txn.exec(sql);
    txn.commit();
    unguard();
    return fmt::format("Table {} deleted successfully", table_name);
}

expected<void, std::string> PgVector::write_content(const std::string& table_name, const std::string& content, const vdb::vector& embedding) {
    guard("PgVector::write_content");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    std::string sql = fmt::format("INSERT INTO {} (content, embedding) VALUES ($1, $2)", table_name);
    txn.exec_params(sql, content, (std::ostringstream() << embedding).str());
    txn.commit();
    unguard();
    return {};
}

json pqxx_result_to_json(const pqxx::result& result) {
    json json_result = json::array();
    for (const auto& row : result) {
        json json_row;
        for (int colnum = 0; colnum < row.size(); ++colnum) {
            json_row[result.column_name(colnum)] = row[colnum].c_str();
        }
        json_result.push_back(json_row);
    }
    return json_result;
}

expected<json, std::string> PgVector::search_content(const std::string& table_name, 
    const vdb::vector& search_vector, int limit, vdb::QueryType type) {
    guard("PgVector::search_content");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    const auto& query_info = query_options[type];
    std::string sql = render_template(query_info.sql_clause, json::object({
        { "table_name"          , table_name            },
        //{ "distance_function"   , L1Distance            }, /// v0.7.0
        //{ "distance_function"   , L2Distance            },
        //{ "distance_function"   , NegativeInnerProduct  },
        { "distance_function"   , CosineDistance        },
        { "limit"               , to_string(limit)      }
    }));

    pqxx::result res = txn.exec_params(sql, (std::ostringstream() << search_vector).str());
    txn.commit();

    json j_res = pqxx_result_to_json(res);

    std::cout << j_res.dump(4);

    json results = json::array();
    for (auto row : res) {
        json item;
        item["content"] = row["content"].c_str();
        if (query_info.result_field) {
            item[*query_info.result_field] = row[*query_info.result_field].as<double>();
        }
        results.push_back(item);
    }
    return results;
    unguard();
    return unexpected<std::string>("Nothing found");
}
