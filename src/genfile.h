#pragma once

#include "core.h"

/*
    agent file format spec

    some_file.gen
    -----------------------
    # root
    ## use: some_label, some_label_2
    some prompt {{some_variable}} text
    some some_label some text some text
    some some_label_2 text some text

    # some_label
    ## use: some_label_2
    some prompt text {{some_variable_2}}
    some prompt text some text some text
    some some_label_2 text some text

    # some_label_2
    some prompt text some text some text
    {{some_variable}} some text some text
    some text {{some_variable_2}} some text
    ...

    {{some_variable}}
    some variable value some variable value
    some variable value
    some value
    {{/some_variable}}

    {{some_variable_2}}
    some variable value
    {{/some_variable_2}}

*/

///
/// @brief Load and parse agent files
///
class GenFile {

    private:

        std::string remove_comment_lines(const std::string& text) {
            std::istringstream input_stream(text);
            std::ostringstream output_stream;
            std::string line;
            while (std::getline(input_stream, line)) {
                if (line.substr(0, 3) != "///") {
                    output_stream << line << '\n';
                }
            }
            return output_stream.str();
        }

        /// Directive parsing

        std::string parse_directive_input(std::string& text, const std::string& default_input) {
            std::istringstream input_stream(text);
            std::ostringstream output_stream;
            std::string line;
            std::string extracted_input = default_input;
            std::regex input_regex(R"(##\s*input:\s*(.+)\s*)");
            while (std::getline(input_stream, line)) {
                std::smatch match;
                if (std::regex_match(line, match, input_regex)) {
                    extracted_input = match[1];
                } else {
                    output_stream << line << "\n";
                }
            }
            text = output_stream.str();
            return extracted_input;
        }

        std::vector<std::string> parse_directive_use(std::string& text) {
            std::istringstream input_stream(text);
            std::ostringstream output_stream;
            std::string line;
            std::vector<std::string> names;
            bool in_use_section = false;
            std::string use_section;
            /// Regular expression to match "## Use:" or "## use:" directive
            std::regex use_regex(R"(##\s*use:\s*(.*))", std::regex::icase);
            while (std::getline(input_stream, line)) {
                std::smatch match;
                if (in_use_section) {
                    if (line.empty() || line.find_first_not_of(' ') == std::string::npos || line[0] == ' ') {
                        use_section += " " + line;
                    } else {
                        std::istringstream names_stream(use_section);
                        std::string name;
                        while (std::getline(names_stream, name, ',')) {
                            names.push_back(trim(name));
                        }
                        use_section.clear();
                        in_use_section = false;
                        output_stream << line << "\n";
                    }
                } else if (std::regex_search(line, match, use_regex)) {
                    in_use_section = true;
                    use_section = match[1].str();
                } else {
                    output_stream << line << "\n";
                }
            }
            if (!use_section.empty()) {
                std::istringstream names_stream(use_section);
                std::string name;
                while (std::getline(names_stream, name, ',')) {
                    names.push_back(trim(name));
                }
            }
            text = output_stream.str();
            return names;
        }

        bool parse_directive_keep_context(std::string& text) {
            std::istringstream input_stream(text);
            std::ostringstream output_stream;
            std::string line;
            /// Default value
            bool keep_context = true;
            /// Regex to match "## keep_context:" followed by optional whitespace and "true" or "false"
            std::regex keep_context_regex(R"(##\s*keep_context:\s*(true|false)\s*)");
            while (std::getline(input_stream, line)) {
                std::smatch match;
                if (std::regex_match(line, match, keep_context_regex)) {
                    if (match[1] == "true") {
                        keep_context = true;
                    } else if (match[1] == "false") {
                        keep_context = false;
                    }
                } else {
                    output_stream << line << "\n";
                }
            }
            text = output_stream.str();
            return keep_context;
        }

        int parse_directive_max_context(std::string& text) {
            std::istringstream input_stream(text);
            std::ostringstream output_stream;
            std::string line;
            /// Default value
            int max_context = 0;
            /// Regex to match "## max_context:" followed by optional whitespace and an integer
            std::regex message_queue_regex(R"(##\s*max_context:\s*(\d+)\s*)");
            while (std::getline(input_stream, line)) {
                std::smatch match;
                if (std::regex_match(line, match, message_queue_regex)) {
                    max_context = std::stoi(match[1].str());
                } else {
                    output_stream << line << "\n";
                }
            }
            text = output_stream.str();
            return max_context;
        }
    
        /// @brief Parse variable sections from gpt content
        /// @param gen_content 
        /// @return Parsed variables as a map array
        std::map<std::string, std::string> parse_variable_sections(std::string& gen_content) {
            std::map<std::string, std::string> variables;
            std::regex var_section(R"(\{\{(\w+)\}\}([\s\S]*?)\{\{/\1\}\})");
            std::smatch matches;
            /// Search for the pattern in the text
            while (std::regex_search(gen_content, matches, var_section)) {
                std::string key = matches[1].str(); /// The variable name
                std::string value = matches[2].str(); /// The variable value between the tags
                value.erase(0, value.find_first_not_of("\r\n")); /// Trim leading newlines
                value.erase(value.find_last_not_of("\r\n") + 1); /// Trim trailing newlines
                variables[key] = value; // Save in the map
                /// Remove the matched block from the content
                gen_content.erase(matches.position(0), matches.length(0));
            }
            /// Remove trailing new lines
            std::regex trailing_newlines_re("[\r\n]+$");
            gen_content = std::regex_replace(gen_content, trailing_newlines_re, "");
            /// Save instructions as a string w/o variables definitions
            return variables;
        }

        /// @brief Parse instructions sections from gpt content
        /// @param gen_content Text data
        /// @return Parsed instructions as a map array
        std::map<std::string, Instruction> parse_instructions(const std::string& gen_content) {
            std::map<std::string, Instruction> instructions;
            /// Regular expression to match mentions and their associated text
            std::regex section_regex(R"(# (\w+)\s*\n([\s\S]*?)(?=\n# \w+\s*\n|$))");
            auto section_begin = std::sregex_iterator(gen_content.begin(), gen_content.end(), section_regex);
            auto section_end = std::sregex_iterator();
            /// Iterate over all matches and populate the map
            for (std::sregex_iterator i = section_begin; i != section_end; ++i) {
                std::smatch match = *i;
                std::string label = match[1].str();
                std::string prompt = match[2].str();
                /// Trim the last newline if it exists
                if (!prompt.empty() && prompt.back() == '\n') {
                    prompt.pop_back();
                }
                ///
                std::string default_input = "Content in a plain text to send to the function.";
                std::string input_prompt = parse_directive_input(prompt, default_input);
                auto use = parse_directive_use(prompt);
                bool keep_context = parse_directive_keep_context(prompt);
                int max_context = parse_directive_max_context(prompt);
                ///
                instructions[label] = Instruction{
                    label,          /// Instruction label
                    prompt,         /// Instruction text
                    input_prompt,   /// Input instruction to generate message
                    0.1,            /// Temp : float
                    use,            /// Use : array
                    keep_context,   /// Keep context : boolean
                    max_context     /// Message queue
                };
            }
            return instructions;
        }

    public:

        /// @brief Load agent file from disk
        /// @param filename Path to the file
        /// @return Tuple with variables and instructions
        std::tuple<std::map<std::string, std::string>, std::map<std::string, Instruction>>
        load_from_file(const std::string& filename) {
            std::map<std::string, std::string> variables;
            std::map<std::string, Instruction> instructions;
            guard("GenFile::load_from_file")
            if (!has_extension(filename, "gen")) {
                throw std::runtime_error("Only .gen files are supported");
            }
             /// Open the file
            std::string gen_file = read_file(filename);
            /// Remove all comments
            gen_file = remove_comment_lines(gen_file);
            /// Crop variable sections from gpt content
            /// and build a map object with variables
            variables = parse_variable_sections(gen_file);
            /// Parse mention sections and return as instructions
            instructions = parse_instructions(gen_file);
            unguard()
            return std::make_tuple(variables, instructions);
        }

};
