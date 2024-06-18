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
    return {};
    unguard();
    return unexpected<std::string>("");
}

json pqxx_result_to_json(const pqxx::result& result) {
    json j_result = json::array();
    for (const auto& row : result) {
        json json_row;
        for (int colnum = 0; colnum < row.size(); ++colnum) {
            json_row[result.column_name(colnum)] = row[colnum].c_str();
        }
        j_result.push_back(json_row);
    }
    return j_result;
}

expected<json, std::string> PgVector::list_collections() {
    guard("PgVector::list_collections");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec("SELECT tablename FROM pg_tables WHERE schemaname = 'public';");
    json j_result = pqxx_result_to_json(result); /*json::array();
    for (const auto& row : result) {
        j_result.push_back(row[0].c_str());
    }*/
    return j_result;
    unguard();
    return unexpected<std::string>("Error when fetching the collection list");
}

expected<json, std::string> PgVector::create_collection(const std::string& table_name, EmbeddingModel model) {
    guard("PgVector::create_collection");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec(fmt::format(
        "SELECT EXISTS (SELECT FROM pg_tables WHERE tablename = '{}');",
        table_name
    ));
    if (result[0][0].as<bool>()) {
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
    txn.exec0("CREATE TABLE IF NOT EXISTS metadata (table_name TEXT PRIMARY KEY, model_name TEXT, dimensions INT)");
    std::string model_name = fmt::format("{}", model);
    txn.exec0(fmt::format(
        "INSERT INTO metadata (table_name, model_name, dimensions) VALUES ('{}', '{}', {})",
        table_name, model_name, vector_dim
    ));
    json j_result = {
        { "table_name", table_name },
        { "model_name", model_name },
        { "dimensions", vector_dim }
    };
    txn.commit();
    return j_result;
    unguard();
    return unexpected<std::string>("");
}

expected<std::string, std::string> PgVector::delete_collection(const std::string& table_name) {
    guard("PgVector::delete_collection");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    std::string metadata_sql = fmt::format("DELETE FROM metadata WHERE table_name = '{}';", table_name);
    txn.exec0(metadata_sql);
    std::string drop_sql = fmt::format("DROP TABLE IF EXISTS {};", table_name);
    txn.exec0(drop_sql);
    txn.commit();
    return fmt::format("Table {} and its metadata deleted successfully", table_name);
    unguard();
    return unexpected<std::string>("");
}

expected<json, std::string> PgVector::get_collection_info(const std::string& table_name) {
    guard("PgVector::get_collection_info");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);    
    pqxx::result exists_result = txn.exec(fmt::format("SELECT EXISTS (SELECT FROM pg_tables WHERE tablename = '{}');", table_name));
    if (!exists_result[0][0].as<bool>()) {
        return unexpected<std::string>(fmt::format("Table {} does not exist", table_name));
    }
    pqxx::result metadata_result = txn.exec(fmt::format("SELECT model_name, dimensions FROM metadata WHERE table_name = '{}';", table_name));
    if (metadata_result.empty()) {
        return unexpected<std::string>("No metadata found for the table");
    }
    json j_result = {
        { "table_name", table_name                                  },
        { "model_name", metadata_result[0][0].c_str()               },
        { "dimensions", to_string(metadata_result[0][1].as<int>())  }
    };
    txn.commit();
    return j_result;
    unguard();
    return unexpected<std::string>("");
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
    ///json j_res = pqxx_result_to_json(res);
    ///std::cout << j_res.dump(4) << "\n\n";
    json j_result = json::array();
    for (auto row : res) {
        json item;
        item["content"] = row["content"].c_str();
        if (query_info.result_field) {
            item[*query_info.result_field] = row[*query_info.result_field].as<double>();
        }
        j_result.push_back(item);
    }
    return j_result;
    unguard();
    return unexpected<std::string>("Nothing found");
}
