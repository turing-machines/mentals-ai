#pragma once

#include "core.h"

class ControlUnit;

using function_t = std::function<std::string(std::shared_ptr<ControlUnit>, const json&)>;

struct ToolCall {
    static std::atomic<int> id_counter;
    int id;
    std::string name;
    json params;
    std::optional<std::string> result;

    ToolCall() : id(id_counter++) {}
    ToolCall(std::string name, json params, std::optional<std::string> result = std::nullopt)
        : id(id_counter++), name(std::move(name)), params(std::move(params)), result(std::move(result)) {}
    ToolCall(int id, std::string name, json params, std::optional<std::string> result = std::nullopt)
        : id(id), name(std::move(name)), params(std::move(params)), result(std::move(result)) {}

    friend void to_json(json& j, const ToolCall& t) {
        j = json{
            {"id", t.id},
            {"name", t.name},
            {"params", t.params},
            {"result", t.result.has_value() ? json(t.result.value()) : json(nullptr)}
        };
    }

    friend void from_json(const json& j, ToolCall& t) {
        ///j.at("id").get_to(t.id);
        j.at("name").get_to(t.name);
        j.at("params").get_to(t.params);
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
    std::shared_ptr<ControlUnit> ce_ref;
    std::unordered_map<std::string, function_t> tools;
    std::vector<std::future<ToolCall>> futures;

public:
    explicit ToolExecutor(std::shared_ptr<ControlUnit> ce) : ce_ref(ce) {}
    void register_tool(const std::string& name, function_t func);
    /// Sync call
    std::optional<std::string> call(const ToolCall& tool_call);
    /// Async call
    void async_batch_call(const std::vector<ToolCall>& batch_tool_call);
    std::vector<ToolCall> async_results();
};
