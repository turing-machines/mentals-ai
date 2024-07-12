#include "tool_executor.h"

/// Register tool with a given name
void ToolExecutor::register_tool(const std::string& name, function_t func) {
     tools.emplace(name, std::move(func));
}

/// 
std::optional<std::string> ToolExecutor::call_tool(const std::string& name, const json& args) {
    auto it = tools.find(name);
    if (it != tools.end()) {
        return it->second(ce_ref, args);  // Pass the shared_ptr
    }
    return std::nullopt;
}
