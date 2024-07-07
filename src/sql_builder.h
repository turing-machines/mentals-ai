#pragma once

#include "core.h"

class SQLBuilder {
public:
    class Builder;

    std::string get_query() const {
        return query;
    }

private:
    std::string query;
    SQLBuilder(const std::string& q) : query(q) {}
};

class SQLBuilder::Builder {
public:
    Builder& select(const std::string& columns) {
        query = fmt::format("SELECT {}", columns);
        return *this;
    }

    Builder& from(const std::string& table_name) {
        query += fmt::format(" FROM {}", table_name);
        return *this;
    }

    Builder& where(const std::string& condition) {
        query += fmt::format(" WHERE {}", condition);
        return *this;
    }

    Builder& exists() {
        query = fmt::format("SELECT EXISTS ({})", query);
        return *this;
    }

    Builder& create_table(const std::string& table_name) {
        query = fmt::format("CREATE TABLE IF NOT EXISTS {} (", table_name);
        return *this;
    }

    Builder& add_column(const std::string& column_definition) {
        if (query.back() != '(') {
            query += ", ";
        }
        query += column_definition;
        return *this;
    }

    Builder& end_table() {
        query += ")";
        return *this;
    }

    Builder& insert_into(const std::string& table_name, const std::string& columns) {
        query = fmt::format("INSERT INTO {} ({}) VALUES (", table_name, columns);
        return *this;
    }

    Builder& values(const std::string& values) {
        query += values + ")";
        return *this;
    }

    Builder& delete_from(const std::string& table_name) {
        query = fmt::format("DELETE FROM {}", table_name);
        return *this;
    }

    Builder& drop_table(const std::string& table_name) {
        query = fmt::format("DROP TABLE IF EXISTS {}", table_name);
        return *this;
    }

    Builder& order_by(const std::string& column, const std::string& order = "ASC") {
        query += fmt::format(" ORDER BY {} {}", column, order);
        return *this;
    }

    Builder& limit(int num_rows) {
        query += fmt::format(" LIMIT {}", num_rows);
        return *this;
    }

    SQLBuilder build() {
        return SQLBuilder(query);
    }

private:
    std::string query;
};
