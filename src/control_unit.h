#pragma once

#include "core.h"
#include "logger.h"
#include "llm.h"
#include "context.h"
#include "memory_controller.h"
#include "tool_executor.h"
#include "native_tools.h"

///#include "code_interpreter.h"

/// Refactoring from AgentExecutor to ControlUnit

#define MAX_TOOLS 5
#define MEMORY_TOOLS "tools"
#define MEMORY_MESSAGES "messages"

class ControlUnit : public std::enable_shared_from_this<ControlUnit>{
private:
    Logger* logger;

    std::unique_ptr<LLMClient> __llm;
    std::unique_ptr<MemoryController> __memc;

    std::unique_ptr<Context> ctx;

    std::string control_unit_instructions;
    std::unique_ptr<ToolExecutor> tools;

    json control_unit_state;

public:
    void init() {
        guard("ControlUnit::init")
    
        control_unit_instructions = read_file("mentals_system.prompt");
        if (control_unit_instructions.empty()) {
            throw "Failed to load control unit instructions";
        }

        ctx = std::make_unique<Context>();
        ctx->add_message("system", "system", control_unit_instructions);

        /*tools = std::make_unique<ToolExecutor>(shared_from_this());

        tools->register_tool("memory"                , tool_memory                  );
        tools->register_tool("read_file"             , tool_read_file               );
        tools->register_tool("write_file"            , tool_write_file              );
        tools->register_tool("append_file"           , tool_append_file             );
        tools->register_tool("send_message"          , tool_send_message            );
        tools->register_tool("user_input"            , tool_user_input              );
        tools->register_tool("execute_bash_command"  , tool_execute_bash_command    );
        tools->register_tool("execute_python_script" , tool_execute_python_script   );*/

        control_unit_state["platform_info"] = "";

        unguard()
    }

public:
    ControlUnit(std::unique_ptr<LLMClient> llm, std::unique_ptr<MemoryController> memc) 
        : __llm(std::move(llm)), __memc(std::move(memc)) {
        logger = Logger::get_instance();
    }

    ~ControlUnit() = default;

    ControlUnit(const ControlUnit&) = delete;
    ControlUnit& operator=(const ControlUnit&) = delete;

    ControlUnit(ControlUnit&&) = default;
    ControlUnit& operator=(ControlUnit&&) = default;

    void process_message(const std::string& message) {

        /// Add message to context
        /// Calculate vector for message
        /// Store message into vdb memory_stream collection
        /// Fetch tools by vector
        /// Update system prompt in context with fetched tools
        /// Call LLMClient with context
        /// Parse response
        /// Execute tools if needed
        /// Add assistant message to context

        control_unit_state["current_date"] = get_current_date();

        std::string name = "user";
        ctx->add_message(name, name, message);
        __memc->process_chunks({ message } , name);
        __memc->write_chunks(MEMORY_MESSAGES, false);
        auto tools_result = __memc->read_chunks(MEMORY_TOOLS, message, MAX_TOOLS);
        __memc->buf_clear();
        if (tools_result.has_value()) {
            std::vector<mem_chunk> tools_chunks = tools_result.value();
            json fetched_tools = json::array();
            std::string few_shot;
            for (auto& tool_chunk : tools_chunks) {
                json j_tool = json::parse(tool_chunk.content);
                std::string one_shot = "```json\n{\n\t\"name\" : \"" + std::string(j_tool["name"]) + "\"";
                for (const auto& param : j_tool["parameters"]) {
                    one_shot += ",\n\t\"" + std::string(param["name"]) + 
                    "\" : \"" + std::string(param["description"]) + "\",";
                }
                one_shot += "\n}\n```";
                few_shot += one_shot + "\n\n";
                fetched_tools.push_back(j_tool);
            }
            control_unit_state["tools"] = fetched_tools.dump(4);
            control_unit_state["tool_call_few_shot"] = few_shot;
            fmt::print("Fetched tools:\n{}\n\n", fetched_tools.dump(4));
        } else {
            control_unit_state["tools"] = "nothing";
            control_unit_state["tool_call_few_shot"] = "nothing";
            fmt::print("Fetched tools:\n{}\n\n", "nothing found");
        }

        ///std::string system_prompt = render_template(control_unit_instructions, control_unit_state);
        std::string system_prompt = inja::render(control_unit_instructions, control_unit_state);
        std::vector<Message> system_messages = ctx->select_messages_by_role("system");
        if (!system_messages.empty()) {
            ctx->update_message(system_messages[0], system_prompt);
        }
        //json j_ctx = *ctx;
        //std::cout << j_ctx.dump(4) << "\n\n";

        /// Call llm
        Response response = __llm->chat_completion(*ctx, 0.5);
        json content = json::parse(std::string(response.content));
        if (content.contains("choices")) {
            for (auto& choice : content["choices"].items()) {
                if (choice.value().contains("message")) {
                    json message = choice.value()["message"];
                    std::string content = std::string(message["content"]);
                    if (!content.empty()) {
                        fmt::print("\n-----\n{}\n\n", content);
                    }
                }
            }
        }

    }

};
