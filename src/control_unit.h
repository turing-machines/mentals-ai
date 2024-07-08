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
private:
    Logger* logger;

    std::unique_ptr<LLM> __llm;
    std::unique_ptr<MemoryController> __memc;

    std::unique_ptr<Context> ctx;

    std::string control_unit_instructions;
    std::unique_ptr<ToolRegistry> tools;

    void init() {
        guard("ControlUnit::init")

        ctx = std::make_unique<Context>();
    
        control_unit_instructions = read_file("mentals_system.prompt");
        if (control_unit_instructions.empty()) {
            throw "Failed to load control unit instructions";
        }

        tools = std::make_unique<ToolRegistry>();

        tools->register_tool("memory"                , tool_memory                  );
        tools->register_tool("read_file"             , tool_read_file               );
        tools->register_tool("write_file"            , tool_write_file              );
        tools->register_tool("append_file"           , tool_append_file             );
        tools->register_tool("send_message"          , tool_send_message            );
        tools->register_tool("user_input"            , tool_user_input              );
        tools->register_tool("execute_bash_command"  , tool_execute_bash_command    );
        tools->register_tool("execute_python_script" , tool_execute_python_script   );

        unguard()
    }

public:
    ControlUnit(std::unique_ptr<LLM> llm, std::unique_ptr<MemoryController> memc) 
        : __llm(std::move(llm)), __memc(std::move(memc)) {
        logger = Logger::get_instance();
        init();
    }

    ~ControlUnit() = default;

    ControlUnit(const ControlUnit&) = delete;
    ControlUnit& operator=(const ControlUnit&) = delete;

    ControlUnit(ControlUnit&&) = default;
    ControlUnit& operator=(ControlUnit&&) = default;

};
