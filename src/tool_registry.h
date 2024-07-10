#pragma once

#include "core.h"

class ControlUnit;

using function_t = std::function<std::string(std::shared_ptr<ControlUnit>, const json&)>;

///
/// @brief Native tools registry
///
class ToolRegistry {
private:
    ///
    std::shared_ptr<ControlUnit> ce_ref;
    std::unordered_map<std::string, function_t> tools;

public:

    explicit ToolRegistry(std::shared_ptr<ControlUnit> ce) : ce_ref(ce) {}

    void register_tool(const std::string& name, function_t func);
    std::optional<std::string> call_tool(const std::string& name, const json& args);
};
