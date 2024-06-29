#include "docfile.h"

expected<void, std::string> DocFile::open(const std::string& file_path) {
    guard("DocFile::open");
    __file_path = file_path;
    Poco::Process::Args args;
    args.push_back("--headless");
    args.push_back("--convert-to");
    args.push_back("txt:Text");
    args.push_back(file_path.c_str());
    Poco::ProcessHandle ph = Poco::Process::launch("/usr/bin/soffice", args);
    ph.wait();
    if (ph.wait() != 0) {
        return unexpected<std::string>("Error: Unable to open document file: " + file_path);
    }
    unguard();
    return {};
}

void DocFile::close() {}

expected<std::string, std::string> DocFile::read() {
    guard("DocFile::read");
    if (__file_path.empty()) {
        return unexpected<std::string>("Error: No document file is open.");
    }
    std::string text_file_path = __file_path + ".txt";
    std::ifstream text_file(text_file_path);
    if (!text_file.is_open()) {
        return unexpected<std::string>("Error: Unable to read text content from document.");
    }
    std::stringstream content;
    content << text_file.rdbuf();
    text_file.close();
    std::remove(text_file_path.c_str());
    return content.str();
    unguard();
    return {};
}

expected<std::string, std::string> DocFile::read(const std::string& file_path) {
    auto open_res = open(file_path);
    if (!open_res) {
        unexpected(open_res.error());
    }
    auto read_res = read();
    if (!read_res) {
        unexpected(read_res.error());
    }
    return read_res.value();
}
