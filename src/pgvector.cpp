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

std::unique_ptr<pqxx::work> PgVector::create_transaction() {
    return std::make_unique<pqxx::work>(*conn);
}

void PgVector::commit_transaction(std::unique_ptr<pqxx::work>& txn) {
    txn->commit();
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

expected<json, std::string> PgVector::create_collection(const std::string& table_name, embedding_model model) {
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
        "CREATE TABLE IF NOT EXISTS {} ("
        "id bigserial PRIMARY KEY, "
        "content_id TEXT, "
        "name TEXT, "
        "meta TEXT, "
        "content TEXT, "
        "embedding vector({}), "
        "created_at TIMESTAMP(3) DEFAULT CURRENT_TIMESTAMP(3))",
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

expected<void, std::string> PgVector::write_content(
    pqxx::work& txn,
    const std::string& table_name,
    const std::string& content_id,
    const int chunk_id,
    const std::string& content, 
    const vdb::vector& embedding, 
    const std::optional<std::string>& name, 
    const std::optional<std::string>& desc
) {
    guard("PgVector::write_content");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    std::string sql = fmt::format(
        "INSERT INTO {} (content_id, name, meta, content, embedding) VALUES ($1, $2, $3, $4, $5)", 
        table_name
    );
    std::string name_value = name ? *name : "";
    std::string desc_value = desc ? *desc : "";
    txn.exec_params(sql, content_id, name_value, desc_value, content, 
        (std::ostringstream() << embedding).str());
    unguard();
    return {};
}

expected<void, std::string> PgVector::write_content(
    pqxx::work& txn,
    const std::string& table_name,
    const mem_chunk& chunk
) {
    return write_content(txn, table_name,
        chunk.content_id, chunk.chunk_id, /// Content address in memory content_id:chunk_id
        chunk.content, chunk.embedding,
        chunk.name, chunk.meta);
}

expected<json, std::string> PgVector::search_content(
    const std::string& table_name, 
    const vdb::vector& search_vector,
    int limit, vdb::query_type type
) {
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
    json j_result = json::array();
    for (auto row : res) {
        json item;
        item["content_id"] = row["content_id"].c_str();
        item["content"] = row["content"].c_str();
        item["name"] = row["name"].c_str();
        item["meta"] = row["meta"].c_str();
        item["created_at"] = row["created_at"].c_str();
        if (query_info.result_field) {
            item[*query_info.result_field] = row[*query_info.result_field].as<double>();
        }
        j_result.push_back(item);
    }
    return j_result;
    unguard();
    return unexpected<std::string>("Nothing found");
}
