#pragma once

#include "core.h"

class ControlUnit;

using function_t = std::function<std::string(std::shared_ptr<ControlUnit>, const json&)>;

struct ToolCall {
    static std::atomic<int> id_counter;
    int id;
    std::string name;
    json args;
    std::optional<std::string> result;

    ToolCall() : id(id_counter++) {}
    ToolCall(std::string name, json args, std::optional<std::string> result = std::nullopt)
        : id(id_counter++), name(std::move(name)), args(std::move(args)), result(std::move(result)) {}
    ToolCall(int id, std::string name, json args, std::optional<std::string> result = std::nullopt)
        : id(id), name(std::move(name)), args(std::move(args)), result(std::move(result)) {}

    friend void to_json(json& j, const ToolCall& t) {
        j = json{
            {"id", t.id},
            {"name", t.name},
            {"args", t.args},
            {"result", t.result.has_value() ? json(t.result.value()) : json(nullptr)}
        };
    }

    friend void from_json(const json& j, ToolCall& t) {
        ///j.at("id").get_to(t.id);
        j.at("name").get_to(t.name);
        j.at("args").get_to(t.args);
        if (j.contains("result") && !j.at("result").is_null()) {
            t.result = j.at("result").get<std::string>();
        } else {
            t.result = std::nullopt;
        }
    }
};


///
/// @brief Tool executor
///
class ToolExecutor {
private:
    ///
    std::shared_ptr<ControlUnit> __ctrl;
    std::unordered_map<std::string, function_t> tools;
    std::vector<std::future<ToolCall>> futures;

public:
    explicit ToolExecutor(std::shared_ptr<ControlUnit> cu) : __ctrl(std::move(cu)) {}

    void register_tool(const std::string& name, function_t func);
    /// Sync call
    std::optional<std::string> invoke_tool(const ToolCall& tool_call);
    /// Async call
    void execute_async_batch(const std::vector<ToolCall>& batch_tool_call);
    std::vector<ToolCall> fetch_async_results();
};
