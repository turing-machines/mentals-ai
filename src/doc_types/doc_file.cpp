#include "doc_file.h"

expected<void, std::string> DocFile::open() {
    guard("DocFile::open");
    Poco::Process::Args args;
    args.push_back("--headless");
    args.push_back("--convert-to");
    args.push_back("txt:Text");
    args.push_back(this->__file_path.c_str());
    Poco::ProcessHandle ph = Poco::Process::launch("/usr/bin/soffice", args);
    ph.wait();
    if (ph.wait() != 0) {
        return unexpected<std::string>("Error: Unable to open document file: " + this->__file_path);
    }
    unguard();
    return {};
}

void DocFile::close() {}

expected<StringBuffer, std::string> DocFile::read() {
    guard("DocFile::read");
    if (this->__file_path.empty()) {
        return unexpected<std::string>("Error: No document file is open.");
    }
    std::string text_file_path = this->__file_path + ".txt";
    std::ifstream text_file(text_file_path);
    if (!text_file.is_open()) {
        return unexpected<std::string>("Error: Unable to read text content from document.");
    }
    std::stringstream content;
    StringBuffer buffer;
    content << text_file.rdbuf();
    buffer.append(content.str());
    text_file.close();
    std::remove(text_file_path.c_str());
    return buffer;
    unguard();
    return {};
}

expected<StringBuffer, std::string> DocFile::read(const std::string& file_path) {
    this->__file_path = file_path;
    auto open_res = open();
    if (!open_res) {
        unexpected(open_res.error());
    }
    auto read_res = read();
    if (!read_res) {
        unexpected(read_res.error());
    }
    return read_res.value();
}
