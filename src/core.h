#pragma once

#include <thread>
#include <atomic>
#include <string>
#include <limits>
#include <random>
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
#include <future>
#include <deque>
#include <algorithm>
#include <functional>
#include <optional>
#include <set>
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
#include <codecvt>
#include <locale>
#include <fmt/core.h>
#include <fmt/format.h>

#include "tl/expected.hpp"
#include "treehh/tree.hh"
#include "nlohmann/json.hpp"
#include "toml++/toml.hpp"
#include "inja/inja.hpp"
#include "meta/meta.hpp"


#define MAX_INTEGER         std::numeric_limits<int>::max()
#define MAKE_SHARED_STRING(str)  std::make_shared<std::string>(str)

#define DEF_TEMP            0.5
#define DEF_TIMEOUT         120000 /// ms
#define SENTENCES_PER_CHUNK 20
#define MAX_TOOLS           5
#define MEMORY_TOOLS        "tools"
#define MEMORY_MESSAGES     "messages"

extern bool debug;
extern std::atomic<bool> spinner_active;
extern std::thread spinner_thread;
extern std::string completion_text;

using std::to_string;
using tl::expected, tl::unexpected;
using json = nlohmann::json;
namespace fs = std::filesystem;

/// ANSI escape codes for coloring text
const std::string RED       = "\033[31m";
const std::string GREEN     = "\033[32m";
const std::string YELLOW    = "\033[33m";
const std::string BLUE      = "\033[34m";
const std::string MAGENTA   = "\033[35m";
const std::string CYAN      = "\033[36m";
const std::string RESET     = "\033[0m";

enum class embedding_model {
    oai_3small = 1536,
    oai_3large = 3072
};

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
std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec);
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
std::vector<json> parse_json_objects(const std::string& text);
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
long long get_timestamp();
std::string get_current_time();
std::mt19937& get_random_engine();
int get_random_number(int min, int max);
std::string gen_index(const std::string& data);
std::string gen_index();
std::vector<std::string> split_text_by_sentences(const std::string& text, int sentences_per_chunk);
bool is_valid_utf8(const std::string &str);
std::string remove_invalid_utf8(const std::string &str);
std::string get_filename_w_ext(const std::string& path);
std::string get_file_extension(const std::string &path);

template <>
struct fmt::formatter<std::vector<std::string>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') {
            throw fmt::format_error("invalid format");
        }
        return it;
    }
    template <typename FormatContext>
    auto format(const std::vector<std::string>& vec, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "{}", vector_to_comma_separated_string(vec));
    }
};

template <>
struct fmt::formatter<embedding_model> : formatter<std::string> {
    auto format(embedding_model c, format_context& ctx) {
        std::string name = "Unknown";
        switch (c) {
            case embedding_model::oai_3small: name = "text-embedding-3-small"; break;
            case embedding_model::oai_3large: name = "text-embedding-3-large"; break;
        }
        return formatter<std::string>::format(name, ctx);
    }
};

namespace vdb {

    enum class query_type {
        embedding,
        distance,
        cosine_similarity
    };

    struct query_info {
        std::string sql_clause;
        std::optional<std::string> result_field;
    };

    class vector {
    public:
        vector() : model(std::nullopt) { allocate_mem(); }
        vector(const std::vector<float>& value, std::optional<embedding_model> model = std::nullopt) 
            : __value(value), model(model) { allocate_mem(); }
        vector(std::vector<float>&& value, std::optional<embedding_model> model = std::nullopt) 
            : __value(std::move(value)), model(model) { allocate_mem(); }
        vector(const float* value, size_t n, std::optional<embedding_model> model = std::nullopt) 
            : __value(value, value + n), model(model) { allocate_mem(); }
        size_t dimensions() const { return __value.size(); }
        std::string get_model_name() const {
            if (model) {
                return fmt::format("{}", *model);
            } else {
                return "No model set";
            }
        }
        void set_model(embedding_model new_model) {
            model = new_model;
            allocate_mem();
        }
        operator const std::vector<float>&() const { return __value; }
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
        std::optional<embedding_model> model;

        void allocate_mem() {
            if (model) {
                size_t required_size = static_cast<size_t>(*model);
                if (__value.capacity() < required_size) {
                    __value.reserve(required_size);
                }
            }
        }

    };
}

struct mem_chunk {
    std::string content_id;
    int chunk_id;
    std::string content;
    vdb::vector embedding;
    std::optional<std::string> name;
    std::optional<std::string> meta;
    std::optional<std::string> created_at;

    mem_chunk() = default;
    mem_chunk(const std::string& content_id, int chunk_id, const std::string& content, 
        const vdb::vector& embedding)
        : content_id(content_id), chunk_id(chunk_id), content(content), embedding(embedding) {}
    mem_chunk(const std::string& content_id, int chunk_id, const std::string& content, 
        const vdb::vector& embedding, const std::optional<std::string>& name)
        : content_id(content_id), chunk_id(chunk_id), content(content), embedding(embedding), 
        name(name) {}
    mem_chunk(const std::string& content_id, int chunk_id, const std::string& content, 
        const std::optional<std::string>& name, const std::optional<std::string>& meta)
        : content_id(content_id), chunk_id(chunk_id), content(content), name(name), meta(meta) {}
    mem_chunk(const std::string& content_id, int chunk_id, const std::string& content, 
        const vdb::vector& embedding, const std::optional<std::string>& name, 
        const std::optional<std::string>& meta)
        : content_id(content_id), chunk_id(chunk_id), content(content), embedding(embedding), 
        name(name), meta(meta) {}
    mem_chunk(const std::string& content_id, int chunk_id, const std::string& content, 
        const vdb::vector& embedding, const std::optional<std::string>& name, 
        const std::optional<std::string>& meta, const std::optional<std::string>& created_at)
        : content_id(content_id), chunk_id(chunk_id), content(content), embedding(embedding), 
        name(name), meta(meta), created_at(created_at) {}

    bool operator==(const mem_chunk& other) const {
        return content_id == other.content_id &&
               chunk_id == other.chunk_id &&
               content == other.content &&
               embedding == other.embedding &&
               name == other.name &&
               meta == other.meta &&
               created_at == other.created_at;
    }

    json serialize_json() const {
        json j;
        j["content_id"] = content_id;
        j["chunk_id"] = chunk_id;
        j["content"] = content;
        ///j["embedding"] = embedding;
        j["name"] = name.value_or(nullptr);
        j["meta"] = meta.value_or(nullptr);
        j["created_at"] = created_at.value_or(nullptr);
        return j;
    }

};

/// Meta

#define REGISTER_CLASS(type, ...) \
template <typename T = type> \
struct __register_##type { \
    __register_##type() { \
        meta_hpp::class_<type>() \
        __VA_ARGS__; \
    } \
}; \
static __register_##type __register_instance_##type;

#define CTOR(...) \
    .constructor_<__VA_ARGS__>()

#define METHOD(method) \
    .method_(#method, &T::method)


///
/// TODO: Refine to work with custom handlers
/// Add diffrenet types of exception handling: info, warning, error, etc.
///
#define guard(method_name) \
    std::string __method_name = method_name; \
    try {

#define unguard() \
    } catch (const std::out_of_range& e) { \
        log_exception(__method_name, fmt::format("Out of range: {}", e.what())); \
        throw; \
    } catch (const std::regex_error& e) { \
        log_exception(__method_name, fmt::format("RegEx error: {}", e.what())); \
        throw; \
    } catch (const json::parse_error& e) { \
        log_exception(__method_name, fmt::format("JSON parse error: {}", e.what())); \
    } catch (const std::runtime_error& e) { \
        log_exception(__method_name, fmt::format("Runtime error: {}", e.what())); \
        throw; \
    } catch (const std::exception& e) { \
        log_exception(__method_name, fmt::format("General exception: {}", e.what())); \
        throw; \
    } catch (...) { \
        log_exception(__method_name, "Unknown exception"); \
        throw; \
    }

#define unguard_fs() \
    } catch (const fs::filesystem_error& e) { \
        log_exception(__method_name, fmt::format("File system error: {}", e.what())); \
        throw; \
    }

