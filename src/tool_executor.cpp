#include "tool_executor.h"

std::atomic<int> ToolCall::id_counter(0);

void ToolExecutor::register_tool(const std::string& name, function_t func) {
    tools.emplace(name, std::move(func));
}

std::optional<std::string> ToolExecutor::call(const ToolCall& tool_call) {
    auto it = tools.find(tool_call.name);
    if (it != tools.end()) {
        return it->second(ce_ref, tool_call.params);
    }
    return std::nullopt;
}

void ToolExecutor::async_batch_call(const std::vector<ToolCall>& batch_tool_call) {
    for (const auto& tool_call : batch_tool_call) {
        futures.emplace_back(std::async(std::launch::async, [this, tool_call]() -> ToolCall {
            auto result = this->call(tool_call);
            return ToolCall{tool_call.id, tool_call.name, tool_call.params, result};
        }));
    }
}

std::vector<ToolCall> ToolExecutor::async_results() {
    std::vector<ToolCall> results;
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    futures.clear();
    return results;
}
