#include "pgvector.h"


PgVector::PgVector(const std::string& conn_info) : conn_str(conn_info), conn(nullptr) {
    logger = Logger::get_instance();
}

PgVector::~PgVector() {}

expected<void, std::string> PgVector::connect() {
    guard("PgVector::connect");
    conn = std::make_unique<pqxx::connection>(conn_str);
    if (!conn->is_open()) {
        logger->log(fmt::format("Failed to connect to database: {}", conn->dbname()));
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

std::vector<mem_chunk> pqxx_result_to_mem_chunks(const pqxx::result& result) {
    std::vector<mem_chunk> chunks;
    for (auto row : result) {
        mem_chunk chunk;
        chunk.content_id = row["content_id"].c_str();
        chunk.chunk_id = row["chunk_id"].as<int>();
        chunk.name = row["name"].c_str();
        chunk.meta = row["meta"].c_str();
        chunk.content = row["content"].c_str();
        chunks.push_back(chunk);
    }
    return chunks;
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
    SQLBuilder::Builder builder;
    SQLBuilder query = builder
        .select("tablename")
        .from("pg_tables")
        .where("schemaname = 'public'")
        .build();
    pqxx::result result = txn.exec(query.get_query());    
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
    SQLBuilder::Builder builder;
    SQLBuilder exists_query = builder
        .select("*")
        .from("pg_tables")
        .where(fmt::format("tablename = '{}'", table_name))
        .exists()
        .build();
    pqxx::result result = txn.exec(exists_query.get_query());
    if (result[0][0].as<bool>()) {
        return unexpected<std::string>(fmt::format("Table {} already exists", table_name));
    }
    txn.exec0("CREATE EXTENSION IF NOT EXISTS vector");
    int vector_dim = static_cast<int>(model);
    SQLBuilder create_table_query = builder
        .create_table(table_name)
        .add_column("id bigserial PRIMARY KEY")
        .add_column("content_id TEXT")
        .add_column("chunk_id INTEGER")
        .add_column("name TEXT")
        .add_column("meta TEXT")
        .add_column("content TEXT")
        .add_column(fmt::format("embedding vector({})", vector_dim))
        .add_column("created_at TIMESTAMP(3) DEFAULT CURRENT_TIMESTAMP(3)")
        .end_table()
        .build();
    txn.exec0(create_table_query.get_query());

    SQLBuilder create_metadata_query = builder
        .create_table("metadata")
        .add_column("table_name TEXT PRIMARY KEY")
        .add_column("model_name TEXT")
        .add_column("dimensions INT")
        .end_table()
        .build();
    txn.exec0(create_metadata_query.get_query());

    std::string model_name = fmt::format("{}", model);
    SQLBuilder insert_metadata_query = builder
        .insert_into("metadata", "table_name, model_name, dimensions")
        .values(fmt::format("'{}', '{}', {}", table_name, model_name, vector_dim))
        .build();
    txn.exec0(insert_metadata_query.get_query());

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
    SQLBuilder::Builder builder;
    SQLBuilder delete_metadata_query = builder
        .delete_from("metadata")
        .where(fmt::format("table_name = '{}'", table_name))
        .build();
    txn.exec0(delete_metadata_query.get_query());
    SQLBuilder drop_table_query = builder
        .drop_table(table_name)
        .build();
    txn.exec0(drop_table_query.get_query());
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
    SQLBuilder::Builder builder;
    SQLBuilder exists_query = builder
        .select("*")
        .from("pg_tables")
        .where(fmt::format("tablename = '{}'", table_name))
        .exists()
        .build();
    pqxx::result exists_result = txn.exec(exists_query.get_query());
    if (!exists_result[0][0].as<bool>()) {
        return unexpected<std::string>(fmt::format("Table {} does not exist", table_name));
    }
    SQLBuilder metadata_query = builder
        .select("model_name, dimensions")
        .from("metadata")
        .where(fmt::format("table_name = '{}'", table_name))
        .build();
    pqxx::result metadata_result = txn.exec(metadata_query.get_query());
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
    SQLBuilder::Builder builder;
    SQLBuilder insert_query = builder
        .insert_into(table_name, "content_id, chunk_id, name, meta, content, embedding")
        .values("$1, $2, $3, $4, $5, $6")
        .build();
    std::string name_value = name ? *name : "";
    std::string desc_value = desc ? *desc : "";
    txn.exec_params(insert_query.get_query(), content_id, chunk_id, name_value, desc_value, content, 
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

expected<std::vector<mem_chunk>, std::string> PgVector::read_content(
    const std::string& table_name,
    const std::optional<std::string>& content_id,
    const std::optional<int>& num_chunks
) {
    guard("PgVector::read_content")
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    SQLBuilder::Builder builder;
    builder.select("content_id, chunk_id, name, meta, content, embedding").from(table_name);
    if (content_id) {
        builder.where(fmt::format("content_id = '{}'", *content_id));
    }
    builder.order_by("chunk_id");
    if (num_chunks) {
        builder.limit(*num_chunks);
    }
    SQLBuilder select_query = builder.build();
    pqxx::work txn(*conn);
    pqxx::result result;
    if (content_id) {
        result = txn.exec_params(select_query.get_query(), *content_id);
    } else {
        result = txn.exec(select_query.get_query());
    }
    txn.commit();
    std::vector<mem_chunk> chunks = pqxx_result_to_mem_chunks(result);
    return chunks;
    unguard()
    return unexpected<std::string>("Nothing found");
}

expected<std::vector<mem_chunk>, std::string> PgVector::read_content(
    const std::string& table_name,
    const std::optional<std::string>& content_id,
    const std::optional<int>& num_chunks,
    const std::optional<std::string>& start_time,
    const std::optional<bool>& before
) {
    guard("PgVector::read_content");
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    SQLBuilder::Builder builder;
    builder.select("content_id, chunk_id, name, meta, content, embedding, created_at").from(table_name);
    if (content_id) {
        builder.where(fmt::format("content_id = '{}'", *content_id));
    }
    if (start_time) {
        if (before && *before) {
            builder.where(fmt::format("created_at < TIMESTAMP '{}'", *start_time));
            builder.order_by("created_at", "DESC");
        } else {
            builder.where(fmt::format("created_at >= TIMESTAMP '{}'", *start_time));
            builder.order_by("created_at", "ASC");
        }
    } else {
        builder.order_by("created_at", "DESC");
    }
    if (num_chunks) {
        builder.limit(*num_chunks);
    }
    SQLBuilder select_query = builder.build();
    pqxx::work txn(*conn);
    pqxx::result result;
    if (content_id) {
        result = txn.exec_params(select_query.get_query(), *content_id);
    } else {
        result = txn.exec(select_query.get_query());
    }
    txn.commit();
    std::vector<mem_chunk> chunks = pqxx_result_to_mem_chunks(result);
    if (chunks.empty()) {
        return unexpected<std::string>("Nothing found");
    }
    return chunks;
    unguard();
    return unexpected<std::string>("");
}

expected<std::vector<mem_chunk>, std::string> PgVector::search_content(
    const std::string& table_name,
    const vdb::vector& search_vector,
    int limit, vdb::query_type type
) {
    guard("PgVector::search_content")
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    std::string distance_function = CosineDistance;
    pqxx::work txn(*conn);
    SQLBuilder::Builder builder;
    if (type == vdb::query_type::embedding) {
        builder.select("*")
            .from(table_name)
            .order_by(fmt::format("embedding {} $1", distance_function))
            .limit(limit);
    } else if (type == vdb::query_type::distance) {
        builder.select(fmt::format("*, embedding {} $1 AS distance", distance_function))
            .from(table_name)
            .order_by("distance")
            .limit(limit);
    } else if (type == vdb::query_type::cosine_similarity) {
        builder.select(fmt::format("*, 1 - (embedding {} $1) AS cosine_similarity", distance_function))
            .from(table_name)
            .order_by("cosine_similarity", "DESC")
            .limit(limit);
    } else {
        return unexpected<std::string>("Invalid query type");
    }
    SQLBuilder query = builder.build();
    pqxx::result result = txn.exec_params(query.get_query(), (std::ostringstream() << search_vector).str());
    txn.commit();
    std::vector<mem_chunk> chunks = pqxx_result_to_mem_chunks(result);
    return chunks;
    unguard()
    return unexpected<std::string>("Nothing found");
}

expected<std::vector<mem_chunk>, std::string> PgVector::bm25_search_content(
    const std::string& table_name,
    const std::string& search_query,
    int limit
) {
    guard("PgVector::bm25_search_content")
    if (!conn || !conn->is_open()) {
        return unexpected<std::string>("Connection to vector db is not open");
    }
    pqxx::work txn(*conn);
    SQLBuilder::Builder builder;    
    builder.select(
        fmt::format(
            "content_id, chunk_id, name, meta, content, embedding, "
            "ts_rank_cd(to_tsvector(content), plainto_tsquery('{}')) AS rank", search_query))
        .from(table_name)
        .where(fmt::format("to_tsvector(content) @@ plainto_tsquery('{}')", search_query))
        .order_by("rank", "DESC")
        .limit(limit);
    SQLBuilder query = builder.build();
    pqxx::result result = txn.exec(query.get_query());
    txn.commit();
    std::vector<mem_chunk> chunks = pqxx_result_to_mem_chunks(result);
    return chunks;
    unguard()
    return unexpected<std::string>("Nothing found");
}
