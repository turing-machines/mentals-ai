#include "txt_file.h"

expected<void, std::string> TxtFile::open() {
    guard("TxtFile::open");
    __file_stream.open(this->__file_path);
    if (!__file_stream.is_open()) {
        return unexpected<std::string>("Error: Unable to open text file: " + this->__file_path);
    }
    is_open = true;
    unguard();
    return {};
}

void TxtFile::close() {
    if (__file_stream.is_open()) {
        __file_stream.close();
        is_open = false;
    }
}

expected<std::string, std::string> TxtFile::read() {
    guard("TxtFile::read");
    if (!is_open) {
        return unexpected<std::string>("Error: No text file is open.");
    }
    std::stringstream content;
    std::string line;
    while (std::getline(__file_stream, line)) {
        content << line << "\n";
    }
    return content.str();
    unguard();
    return {};
}

expected<std::string, std::string> TxtFile::read(const std::string& file_path) {
    this->__file_path = file_path;
    auto file_res = open();
    if (!file_res) {
        unexpected(file_res.error());
    }
    auto read_res = read();
    if (!read_res) {
        unexpected(read_res.error());
    }
    close();
    return read_res.value();
}
