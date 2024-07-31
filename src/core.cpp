#include "../include/core.h"


void print_help() {
    std::cout << "\nUsage: mentals <filename> [--input=value] [-d|--debug]\n"
        << "Arguments:\n"
        << "  <filename>    The name of the agent file (.gen) to run.\n"
        << "  --input=value The input value for the agent.\n"
        << "  -h, --help    Show this help message and exit.\n"
        << "  -d, --debug   Output debug messages.\n\n";
}

std::string parse_input(int argc, char* argv[], std::string& input) {
    if (argc == 1 || std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
        print_help();
        exit(EXIT_SUCCESS);
    }
    std::string filename = argv[1];
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.starts_with("--input=")) {
            input = arg.substr(8);
        } else if (arg == "-d" || arg == "--debug") {
            debug = true;
        }
    }
    return filename;
}

std::string get_current_date() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    char date_buffer[11]; // Format: YYYY-MM-DD
    std::strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", local_time);
    return std::string(date_buffer);
}

std::string to_lower(const std::string &str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    return lower_str;
}

std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

int get_terminal_width() {
    std::string result;
    FILE* pipe = popen("stty size", "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    char buffer[128];
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
        result += buffer;
    }
    if (pclose(pipe) == -1) {
        throw std::runtime_error("Failed to close command stream");
    }
    std::istringstream iss(result);
    int rows, cols;
    iss >> rows >> cols;
    return cols;
}

std::string trim_by_terminal_width(const std::string& text) {
    int terminal_width = get_terminal_width();
    auto max_width = static_cast<std::string::size_type>(terminal_width * 0.75);
    std::string trimmed_text = text;
    if (trimmed_text.length() > max_width) {
        trimmed_text = trimmed_text.substr(0, max_width - 3) + "...";
    }
    return trimmed_text;
}

std::string string_in_line(const std::string& text) {
    std::string single_line = std::regex_replace(text, std::regex("[\\n\\t]+"), " ");
    single_line = std::regex_replace(single_line, std::regex("[ ]+"), " ");
    std::string formatted_line = trim_by_terminal_width(single_line);
    return formatted_line;
}

void print_in_line(const std::string& color, const std::string& opcode, const std::string& text) {
    std::string single_line = color + opcode + RESET;
    for (char c : text) {
        if (c != '\n') {
            single_line += c;
        } else {
            single_line += ' ';
        }
    }
    std::string formatted_line = trim_by_terminal_width(single_line);
    std::cout << formatted_line << std::endl;
}

std::string vector_to_comma_separated_string(const std::vector<std::string>& vec) {
    std::string result;
    for (size_t i = 0; i < vec.size(); ++i) {
        result += vec[i];
        if (i != vec.size() - 1) {
            result += ", ";
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& vec) {
    os << vector_to_comma_separated_string(vec);
    return os;
}

std::string escape_json(const std::string& json_str) {
    std::string escaped;
    for (char c : json_str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '/': escaped += "\\/"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

std::string render_template(const std::string& template_str, const std::map<std::string, std::string>& values_map) {
    std::string result = template_str;
    guard("render_template")
    ///
    for (const auto& pair : values_map) {
        std::string template_key = "{{" + pair.first + "}}";
        std::string value = escape_json(pair.second);
        size_t pos = result.find(template_key);
        while (pos != std::string::npos) {
            result.replace(pos, template_key.length(), value);
            pos = result.find(template_key, pos + value.length());
        }
    }
    ///
    unguard()
    return result;
}

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

bool write_file(const std::string& filename, const std::string& content) {
    std::ofstream out_file(filename);
    if (!out_file) {
        return false;
    }
    out_file << content << std::endl;
    if (out_file.fail()) {
        return false;
    }
    out_file.close();
    return true;
}

bool append_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::app);    
    if (file.is_open()) {
        file << content << std::endl;
        file.close();
        return true;
    } else {
        return false;
    }
}

std::string execute_command(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::ostringstream result_stream;
    try {
        FILE* pipe = popen((cmd + " 2>&1").c_str(), "r");
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result_stream << buffer.data();
        }
        int return_code = pclose(pipe);
        if (return_code != 0) {
            //std::ostringstream error_msg;
            //error_msg << "Command failed with return code " << return_code
            //        << ": " << result_stream.str();
            throw std::runtime_error(result_stream.str());
        }
    } catch(const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return e.what();
    }
    return result_stream.str();
}

bool contains_substring(const std::string& text, const std::string& substring) {
    return text.find(substring) != std::string::npos;
}

std::string erase_text_after_specified_substring(const std::string& text, const std::string& substring) {
    size_t pos = text.find(substring);
    if (pos != std::string::npos) {
        /// Found the substring, return everything up to and including the substring
        return text.substr(0, pos + substring.length());
    }
    /// Substring not found, return the original text
    return text;
}

std::string replace_new_lines(const std::string& input) {
    std::string result = input;
    std::string new_line = "\n";
    std::string escaped_new_line = "\\n";
    size_t pos = 0;
    while ((pos = result.find(new_line, pos)) != std::string::npos) {
        result.replace(pos, new_line.length(), escaped_new_line);
        /// Move past the inserted "\\n" to avoid infinite loop
        pos += escaped_new_line.length();
    }
    return result;
}

std::string convert_to_single_line(const std::string& multiline_text) {
    std::string single_line_text = multiline_text;  /// Copy the input text
    size_t position = 0;
    /// Replace all newline characters with spaces
    while ((position = single_line_text.find('\n', position)) != std::string::npos) {
        single_line_text.replace(position, 1, " ");  /// Replace '\n' with a space
        position++;  /// Move past the last replaced position
    }
    return single_line_text;
}

std::vector<std::string> extract_json_blocks(const std::string& text) {
    std::vector<std::string> result;
    guard("extract_json_blocks")
    std::regex json_regex(R"(\s*```json\n(\{[\s\S]*?\})\n\s*```)");
    std::sregex_iterator next(text.begin(), text.end(), json_regex);
    std::sregex_iterator end;
    while (next != end) {
        std::smatch match = *next;
        result.push_back(match[0].str());
        next++;
    }
    unguard()
    return result;
}

std::string extract_json_from_markdown(const std::string& text) {
    guard("extract_json_from_markdown")
    std::regex json_regex(R"(```json\n(\{[\s\S]*?\})\n```)");
    std::smatch match;
    if (std::regex_search(text, match, json_regex)) {
        return match[1].str();
    } else {
        return text;
    }
    unguard()
    return text;
}

std::string get_first_instruction(const std::vector<std::string>& array) {
    for (const auto& str : array) {
        if (str.find("\"name\"") != std::string::npos) {
            return str;
        }
    }
    return "";
}

json find_object_by_field_value(const json& json_array, const std::string& key, const json& value) {
    if (!json_array.is_array()) {
        throw std::runtime_error("The provided JSON is not an array.");
    }
    for (const auto& item : json_array) {
        if (item.is_object() && item.contains(key)) {
            if (item[key] == value) {
                return item;
            }
        }
    }
    return json();
}

json remove_element_by_name(const json& json_array, const std::string& key, const std::string& value) {
    json new_array = json::array();
    for (const auto& item : json_array) {
        if (item.is_object() && item.contains(key)) {
            if (item.at(key) != value) {
                new_array.push_back(item);
            }
        } else {
            new_array.push_back(item);
        }
    }
    return new_array;
}

bool is_json_object(const std::string& str) {
    try {
        std::string modified_text = extract_json_from_markdown(str);
        auto json_object = json::parse(modified_text);
        return json_object.is_object();
    } catch (json::parse_error& e) {
        return false;
    }
}

json parse_to_json(const std::string& str) {
    try {
        //std::string modified_text = convert_to_single_line(text);
        //modified_text = replace_new_lines(modified_text);
        std::string modified_text = extract_json_from_markdown(str);
        json j = json::parse(modified_text);
        return j;
    } catch (json::parse_error& e) {
        std::cerr << RED << "parse_to_json: JSON text to parse: " << str << '\n' << std::endl;
        std::cerr << RED << "JSON parsing error: " << e.what() << '\n'
            << "Exception id: " << e.id << '\n'
            << "Byte position of error: " << e.byte << std::endl;
        return json({}); /// If error return null json object
    }
}

int word_count(const std::string& text) {
    std::istringstream iss(text);
    std::string word;
    int count = 0;
    while (iss >> word) {
        ++count;
    }
    return count;
}

std::string remove_file_extension(const std::string& filename) {
    size_t last_dot = filename.rfind('.');
    if (last_dot == std::string::npos || last_dot == 0) {
        return filename;
    }
    return filename.substr(0, last_dot);
}

bool find_string_in_vector(const std::vector<std::string>& vec, const std::string& target) {
    auto it = std::find(vec.begin(), vec.end(), target);
    return it != vec.end();
}

std::string mask_api_key(const std::string &api_key) {
    size_t length = api_key.length();
    size_t mask_length = length / 2;
    size_t start_mask_pos = (length - mask_length) / 2;
    std::string masked_key = api_key;
    for (size_t i = start_mask_pos; i < start_mask_pos + mask_length; ++i) {
        masked_key[i] = '*';
    }
    return masked_key;
}

std::string user_input() {
    std::string message;
    std::getline(std::cin, message);
    return message;
}

bool stop_token(std::string& text) {
    const std::string stop_token = "<<RETURN>>";
    size_t pos = text.find(stop_token);
    if (pos != std::string::npos) {
        text.erase(pos, stop_token.length());
        return true;
    }
    return false;
}

std::string json_value_to_string(const json& json_obj, const std::string& key) {
    if (json_obj.contains(key)) {
        const auto& field = json_obj[key];
        if (field.is_object() || field.is_array()) {
            return field.dump(4);
        } else if (field.is_string()) {
            return field.get<std::string>();
        } else {
            return field.dump(4);
        }
    }
    return "";
}

void log_exception(const std::string& method, const std::string& message) {
    std::cerr << RED << "Exception in " << method << ": " << message << std::endl;
}

bool has_extension(const std::string& filename, const std::string& extension) {
    if (filename.length() <= extension.length() + 1) {
        return false;
    }
    size_t dot_pos = filename.rfind('.');
    if (dot_pos == std::string::npos || dot_pos == 0) {
        return false;
    }
    std::string found_extension = filename.substr(dot_pos + 1);
    return found_extension == extension;
}

json accumulate_values(const json& j1, const json& j2) {
    json result = j1;
    for (auto& [key, value] : j2.items()) {
        if (result.contains(key) && result[key].is_number_integer() && value.is_number_integer()) {
            result[key] = result[key].get<int>() + value.get<int>();
        } else if (!result.contains(key)) {
            result[key] = value;
        }
    }
    return result;
}

void run_spinner(const std::string& text) {
    const char spin_chars[] = {'|', '/', '-', '\\'};
    int spin_index = 0;
    while (spinner_active) {
        std::cout << RESET << "\r" << text << ".. " << spin_chars[spin_index] << std::flush;
        spin_index = (spin_index + 1) % sizeof(spin_chars);
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    std::cout << "\r" << text << ": " << completion_text << std::flush;
}

void start_spinner(const std::string& text) {
    spinner_active = true;
    spinner_thread = std::thread(run_spinner, text);
}

void stop_spinner(const std::string& text) {
    if (!spinner_active) return;
    completion_text = text;
    spinner_active = false;
    if (spinner_thread.joinable()) {
        spinner_thread.join();
    }
}

void print_node_lines(const std::string& prefix, const std::string& node_text) {
    std::istringstream ss(node_text);
    std::string line;
    bool first_line = true;
    while (std::getline(ss, line)) {
        if (first_line) {
            std::cout << prefix << "- " << line << std::endl;
            first_line = false;
        } else {
            std::cout << prefix << "  " << line << std::endl;
        }
    }
}

void print_tree(const tree<std::string>& tr) {
    tree<std::string>::pre_order_iterator it = tr.begin();
    tree<std::string>::pre_order_iterator end = tr.end();
    if (!tr.is_valid(it)) return;
    int root_depth = tr.depth(it);
    std::cout << "-----" << std::endl;
    while (it != end) {
        int current_depth = tr.depth(it) - root_depth;
        std::string prefix;
        for (int i = 0; i < current_depth; ++i) {
            if (i == current_depth - 1) {
                prefix += "|";
            } else {
                prefix += "|   ";
            }
        }
        print_node_lines(prefix, *it);
        ++it;
    }
    std::cout << "-----" << std::endl;
}

tree<std::string>::pre_order_iterator find_node(const tree<std::string>& tr, const std::string& node_value) {
    auto it = tr.begin();
    auto end = tr.end();
    if (!tr.is_valid(it)) return end;
    while (it != end) {
        if (contains_substring(*it, node_value)) {
            return it;
        }
        ++it;
    }
    return end;
}

bool append_child(tree<std::string>& tr, const std::string& node_value, const std::string& child_value) {
    tree<std::string>::pre_order_iterator it = find_node(tr, node_value);
    if (it != tr.end()) {
        tr.append_child(it, child_value);
        return true;
    }
    return false;
}

long long get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration_since_epoch = now.time_since_epoch();
    auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(duration_since_epoch).count();
    return milliseconds_since_epoch;
}

std::mt19937& get_random_engine() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

int get_random_number(int min, int max) {
    std::uniform_int_distribution<> dist(min, max);
    return dist(get_random_engine());
}

std::string gen_index(const std::string& data) {
    std::hash<std::string> hasher;
    auto hashed = hasher(data);
    std::stringstream ss;
    ss << std::hex << hashed;
    return ss.str().substr(0, 8);
}

std::string gen_index() {
    std::string rnd = to_string(get_random_number(1, MAX_INTEGER));
    std::string tms = to_string(get_timestamp());
    return gen_index(tms+rnd);
}

/// Naive method just for testing purposes
/// TODO: Different strategies e.g. w/ pages, paragraph, overlapping etc. 
std::vector<std::string> split_text_by_sentences(const std::string& text, int sentences_per_chunk) {
    std::vector<std::string> chunks;
    std::istringstream stream(text);
    std::string sentence;
    std::string chunk;
    int count = 0;
    while (std::getline(stream, sentence, '.')) {
        sentence.erase(sentence.begin(), 
            std::find_if(sentence.begin(), sentence.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        if (!sentence.empty()) {
            chunk += sentence + ".";
            count++;
        }
        if (count == sentences_per_chunk) {
            chunks.push_back(chunk);
            chunk = "";
            count = 0;
        }
    }
    if (!chunk.empty()) {
        chunks.push_back(chunk);
    }
    return chunks;
}

bool is_valid_utf8(const std::string &str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    try {
        std::wstring wstr = conv.from_bytes(str);
    } catch (const std::range_error &) {
        return false;
    }
    return true;
}

std::string remove_invalid_utf8(const std::string &str) {
    std::string result;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    for (char ch : str) {
        if ((ch & 0x80) == 0) {
            result += ch;
        } else {
            try {
                std::string tmp(1, ch);
                result += conv.to_bytes(conv.from_bytes(tmp));
            } catch (const std::range_error &) { /*skip invalid utf-8 byte */ }
        }
    }
    return result;
}
