#pragma once

#include "core.h"

/**
struct mem_chunk {
    std::string content_id;
    int chunk_id;
    std::string content;
    vdb::vector embedding;
    std::optional<std::string> name;
    std::optional<std::string> meta;
*/

// 1 message == 1 chunk

struct Message {
    std::string content_id;
    std::string created_at;
    std::string name;
    std::string role;
    std::string content;

    friend void to_json(json& j, const Message& m) {
        j = json{
            { "content_id"  , m.content_id  },
            { "created_at"  , m.created_at  },
            { "name"        , m.name        },
            { "role"        , m.role        },
            { "content"     , m.content     }
        };
    }

    friend void from_json(const json& j, Message& m) {
        j.at("content_id").get_to(m.content_id);
        j.at("created_at").get_to(m.created_at);
        j.at("name").get_to(m.name);
        j.at("role").get_to(m.role);
        j.at("content").get_to(m.content);
    }
};

class Context : public std::vector<Message> {
public:
    Context() = default;
    Context(const json& json_obj) {
        *this = json_obj.get<Context>();
    }
    Context(const Context& other) = default;
    Context(Context&& other) noexcept = default;

    Context& operator=(const Context& other) = default;
    Context& operator=(Context&& other) noexcept = default;

    friend std::ostream& operator<<(std::ostream& os, const Context& context) {
        for (const auto& message : context) {
            os << "Content ID: " << message.content_id << "\n"
               << "Created At: " << message.created_at << "\n"
               << "Name: " << message.name << "\n"
               << "Role: " << message.role << "\n"
               << "Content: " << message.content << "\n---\n";
        }
        return os;
    }

    Context& operator+=(const Context& other) {
        insert(end(), other.begin(), other.end());
        return *this;
    }

    Context operator+(const Context& other) const {
        Context result(*this);
        result += other;
        return result;
    }

    Message& add_message(const Message& message) {
        push_back(message);
        return back();
    }

    Message& add_message(const std::string& name, const std::string& role, const std::string& content) {
        std::string created_at = to_string(get_timestamp());
        std::string random = to_string(get_random_number(1, MAX_INTEGER));
        std::string content_id = gen_index(created_at + random);
        return add_message({ content_id, created_at, name, role, content });
    }

    void delete_message(const std::string& content_id) {
        auto it = std::remove_if(begin(), end(), [&content_id](const Message& m) { 
            return m.content_id == content_id; 
        });
        erase(it, end());
    }

    void update_message(const std::string& content_id, const std::string& new_content) {
        auto it = std::find_if(begin(), end(), [&content_id](const Message& m) { 
            return m.content_id == content_id;
        });
        if (it != end()) {
            it->content = new_content;
        }
    }

    void update_message(const Message& msg, const std::string& new_content) {
        update_message(msg.content_id, new_content);
    }

    std::vector<Message> search_messages(const std::string& text) const {
        std::vector<Message> results;
        for (const auto& message : *this) {
            if (message.content.find(text) != std::string::npos) {
                results.push_back(message);
            }
        }
        return results;
    }

    std::vector<Message> select_messages_by_role(const std::string& role) const {
        std::vector<Message> results;
        for (const auto& message : *this) {
            if (message.role == role) {
                results.push_back(message);
            }
        }
        return results;
    }

    friend void to_json(json& j, const Context& context) {
        j = json::array();
        for (const auto& message : context) {
            j.push_back(message);
        }
    }

    friend void from_json(const json& j, Context& context) {
        context.clear();
        for (const auto& item : j) {
            context.push_back(item.get<Message>());
        }
    }

};
