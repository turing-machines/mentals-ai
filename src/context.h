#pragma once

#include "core.h"


class Context {
public:
    Context() {}
    Context(const json& json_obj) {
        deserialize(json_obj);
    }
    Context(const Context& other) : __messages(other.__messages) {}

    Context& operator+=(const Context& other) {
        for (const auto& message : other.__messages) {
            __messages.push_back(message);
        }
        return *this;
    }

    Context operator+(const Context& other) const {
        Context result(*this);
        result += other;
        return result;
    }

    void add_message(const Message& message) {
        __messages.push_back(message);
    }

    void add_message(const std::string& name, const std::string& role, const std::string& content) {
        std::string timestamp = to_string(get_timestamp());
        std::string index = gen_index(timestamp);
        add_message({ index, timestamp, name, role, content });
    }

    void delete_message(const std::string& index) {
        auto it = std::remove_if(__messages.begin(), __messages.end(), [&index](const Message& m) { return m.index == index; });
        __messages.erase(it, __messages.end());
    }

    void update_message(const std::string& index, const std::string& new_content) {
        auto it = std::find_if(__messages.begin(), __messages.end(), [&index](const Message& m) { return m.index == index; });
        if (it != __messages.end()) {
            it->content = new_content;
        }
    }

    std::vector<Message> search_messages(const std::string& text) const {
        std::vector<Message> results;
        for (const auto& message : __messages) {
            if (message.content.find(text) != std::string::npos) {
                results.push_back(message);
            }
        }
        return results;
    }

    std::vector<Message> select_messages_by_role(const std::string& role) const {
        std::vector<Message> results;
        for (const auto& message : __messages) {
            if (message.role == role) {
                results.push_back(message);
            }
        }
        return results;
    }

    json serialize() const {
        json j = json::array();
        for (const auto& message : __messages) {
            j.push_back(message.to_json());
        }
        return j;
    }

    void deserialize(const json& json_obj) {
        __messages.clear();
        for (const auto& item : json_obj) {
            __messages.push_back(Message::from_json(item));
        }
    }

private:
    std::vector<Message> __messages;

};
