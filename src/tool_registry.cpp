#include "../include/tool_registry.h"

/// Register tool with a given name
void ToolRegistry::register_tool(const std::string& name, function_t func) {
     tools.emplace(name, std::move(func));
}

/// 
std::optional<std::string> ToolRegistry::call_tool(const std::string& name, const json& args) {
    auto it = tools.find(name);
    if (it != tools.end()) {
        return it->second(ce_ref, args);  // Pass the shared_ptr
    }
    return std::nullopt;
}
