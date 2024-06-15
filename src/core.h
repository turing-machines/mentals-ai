#pragma once

#include <thread>
#include <atomic>
#include <string>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <memory>
#include <stack>
#include <algorithm>
#include <functional>
#include <optional>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <tuple>
#include <regex>
#include <cctype>
#include <type_traits>
#include <stdexcept>
#include <cassert>
#include <fmt/core.h>

#include "tl/expected.hpp"
#include "treehh/tree.hh"
#include "nlohmann/json.hpp"
#include "toml++/toml.hpp"

#include <pqxx/pqxx>

extern bool debug;
extern std::atomic<bool> spinner_active;
extern std::thread spinner_thread;
extern std::string completion_text;

using json = nlohmann::json;
using std::to_string;

/// ANSI escape codes for coloring text
const std::string RED       = "\033[31m";
const std::string GREEN     = "\033[32m";
const std::string YELLOW    = "\033[33m";
const std::string BLUE      = "\033[34m";
const std::string MAGENTA   = "\033[35m";
const std::string CYAN      = "\033[36m";
const std::string RESET     = "\033[0m";

/// Instruction spec with properties
struct Instruction {
    std::string label;
    std::string prompt;
    std::string input_prompt;
    float temp;
    std::vector<std::string> use;
    bool keep_context;
    int max_context;
};

void print_help();
std::string parse_input(int argc, char* argv[], std::string& input);
std::string get_current_date();
std::string to_lower(const std::string &str);
std::string trim(const std::string &str);
int get_terminal_width();
std::string trim_by_terminal_width(const std::string& text);
std::string string_in_line(const std::string& text);
void print_in_line(const std::string& color, const std::string& opcode, const std::string& text);
std::string vector_to_comma_separated_string(const std::vector<std::string>& vec);
std::string escape_json(const std::string& json_str);
std::string render_template(const std::string& template_str, const std::map<std::string, std::string>& values_map);
std::string read_file(const std::string& file_path);
bool write_file(const std::string& file_path, const std::string& content);
bool append_file(const std::string& file_path, const std::string& content);
std::string execute_command(const std::string& cmd);
bool contains_substring(const std::string& text, const std::string& substring);
std::string erase_text_after_specified_substring(const std::string& text, const std::string& substring);
std::string replace_new_lines(const std::string& input);
std::string convert_to_single_line(const std::string& multiline_text);
std::vector<std::string> extract_json_blocks(const std::string& text);
std::string extract_json_from_markdown(const std::string& text);
std::string get_first_instruction(const std::vector<std::string>& array);
json find_object_by_field_value(const json& json_array, const std::string& field_name, const json& value);
json remove_element_by_name(const json& json_array, const std::string& field_name, const std::string& value);
bool is_json_object(const std::string& str);
json parse_to_json(const std::string& str);
int word_count(const std::string& text);
std::string remove_file_extension(const std::string& filename);
bool find_string_in_vector(const std::vector<std::string>& vec, const std::string& target);
std::string mask_api_key(const std::string &api_key);
std::string user_input();
bool stop_token(std::string& text);
std::string json_value_to_string(const json& json_obj, const std::string& key);
void log_exception(const std::string& method, const std::string& message);
bool has_extension(const std::string& filename, const std::string& extension);
json accumulate_values(const json& j1, const json& j2);
void run_spinner(const std::string& text);
void start_spinner(const std::string& text);
void stop_spinner(const std::string& text);
void print_tree(const tree<std::string>& tr);
tree<std::string>::pre_order_iterator find_node(const tree<std::string>& tr, const std::string& node_value);
bool append_child(tree<std::string>& tr, const std::string& node_value, const std::string& child_value);

enum class EmbeddingModel {
    ada002 = 1536,
    large3 = 3072
};

namespace vdb {
    class vector {
        public:
            vector() = default;
            vector(const std::vector<float>& value) { __value = value; }
            vector(std::vector<float>&& value) { __value = std::move(value); }
            vector(const float* value, size_t n) { __value = std::vector<float>{value, value + n}; }
            size_t dimensions() const { return __value.size(); }
            operator const std::vector<float>() const { return __value; }
            friend bool operator==(const vector& lhs, const vector& rhs) { return lhs.__value == rhs.__value; }
            friend std::ostream& operator<<(std::ostream& os, const vector& value) {
                os << "[";
                for (size_t i = 0; i < value.__value.size(); i++) {
                    if (i > 0) { os << ","; }
                    os << fmt::format("{:.6f}", value.__value[i]);
                }
                os << "]";
                return os;
            }

        private:
            std::vector<float> __value;
    };
}

#define guard(method_name) \
    std::string __method = method_name; \
    try {

#define unguard() \
    } catch (const std::out_of_range& e) { \
        log_exception(__method, fmt::format("Out of range: {}", e.what())); \
        throw; \
    } catch (const std::regex_error& e) { \
        log_exception(__method, fmt::format("RegEx error: {}", e.what())); \
        throw; \
    } catch (const json::parse_error& e) { \
        log_exception(__method, fmt::format("JSON parse error: {}", e.what())); \
    } catch (const std::runtime_error& e) { \
        log_exception(__method, fmt::format("Runtime error: {}", e.what())); \
        throw; \
    } catch (const std::exception& e) { \
        log_exception(__method, fmt::format("General exception: {}", e.what())); \
        throw; \
    } catch (...) { \
        log_exception(__method, "Unknown exception"); \
        throw; \
    }
