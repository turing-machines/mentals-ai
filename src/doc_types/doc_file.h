#pragma once

#include "file_interface.h"
#include <Poco/Process.h>

class DocFile : public FileInterface {
public:
    explicit DocFile(const std::string& file_path) : FileInterface(file_path) {}
    ~DocFile() override { close(); }

    expected<void, std::string> open() override;
    void close() override;
    expected<StringBuffer, std::string> read() override;
    expected<StringBuffer, std::string> read(const std::string& file_path) override;

};
