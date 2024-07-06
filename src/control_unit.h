#pragma once

#include "core.h"
#include "logger.h"
#include "llm.h"
#include "context.h"
#include "memory_controller.h"
#include "tool_registry.h"
#include "native_tools.h"

///#include "code_interpreter.h"

/// Refactoring from AgentExecutor to ControlUnit

class ControlUnit {
public:

    LLM& llm;
    Context& ctx;

private:
    std::unique_ptr<ToolRegistry> tools;
    std::string system_prompt;
};
