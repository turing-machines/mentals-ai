#pragma once

#include "core.h"
#include "logger.h"
#include "llm.h"
#include "genfile.h"
#include "code_interpreter.h"

class ToolRegistry;

///
/// @brief Instructions execution
///
class CentralExecutive : public std::enable_shared_from_this<CentralExecutive> {
    public:
        /// Natural language processor
        LLM llm;
        /// Python
        CodeInterpreter code_interpreter;
        /// Short term memory is global per thread
        /// All the instructions in this thread have
        /// access to the short term memory data
        json short_term_memory;
        ///
        json central_executive_state;
        /// Stat
        int nlop;
        double nlops;
        long long total_time;
        json usage;
        int toks;

    private:
        ///
        ///friend class ToolRegistry;
        std::unique_ptr<ToolRegistry> tools;
        ///
        std::string central_executive_instruction;
        /// Loaded native instructions
        json native_instructions;
        /// Loaded agent instructions
        json agent_instructions;
        /// Current active working memory
        std::shared_ptr<liboai::Conversation> working_memory;
        ///
        std::map<std::string, std::shared_ptr<liboai::Conversation>> working_contexts;
        std::vector<Instruction> instructions_call_stack;
        /// Agent instructions as a map array: key == instruction name, value == instruction
        std::map<std::string, Instruction> instructions;
        ///
        Logger* logger;

#if defined(__PGVECTOR__)
        pqxx::connection& conn_ref;
#endif

    public:
        CentralExecutive();
#if defined(__PGVECTOR__)
        ControlUnit(pqxx::connection& conn);
#endif

        ~CentralExecutive();
        ///
        void set_state_variable(const std::string& name, const std::string& value);
        bool init_native_tools(const std::string& file_path);
        void init_agent(std::map<std::string, Instruction>& inst);
        std::string run_agent_thread(const std::string& entry_instruction, 
            const std::string& input, std::optional<liboai::Conversation> context = std::nullopt);

    private:
        void add_agent_instruction(const std::string& name, const std::string& description, 
            const std::string& input_prompt);
        void prepare_agent_instructions(int word_count_limit);
        void update_state(const Instruction& instruction);
        void execute();
        void apply_instruction_response(std::shared_ptr<liboai::Conversation> working_memory,
            const std::string& content, const std::string& name, const std::string& result);
        void stop(const std::string &content);
        void parse_content(std::string& content);
};
