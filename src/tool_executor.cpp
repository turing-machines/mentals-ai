#include "tool_executor.h"

std::atomic<int> ToolCall::id_counter(0);

void ToolExecutor::register_tool(const std::string& name, function_t func) {
    tools.emplace(name, std::move(func));
}

std::optional<std::string> ToolExecutor::invoke_tool(const ToolCall& tool_call) {
    guard("ToolExecutor::invoke_tool")
    auto it = tools.find(tool_call.name);
    if (it != tools.end()) {
        return it->second(__ctrl, tool_call.params);
    }
    return std::nullopt;
    unguard()
    return std::nullopt;
}

void ToolExecutor::execute_async_batch(const std::vector<ToolCall>& batch_tool_call) {
    guard("ToolExecutor::execute_async_batch")
    for (const auto& tool_call : batch_tool_call) {
        futures.emplace_back(std::async(std::launch::async, [this, tool_call]() -> ToolCall {
            auto result = this->invoke_tool(tool_call);
            return ToolCall{tool_call.id, tool_call.name, tool_call.params, result};
        }));
    }
    unguard()
}

std::vector<ToolCall> ToolExecutor::fetch_async_results() {
    std::vector<ToolCall> results;
    guard("ToolExecutor::fetch_async_results")
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    futures.clear();
    return results;
    unguard()
    return results;
}
