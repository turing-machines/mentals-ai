#pragma once

#include "file_interface.h"
#include <Poco/Process.h>

class DocFile : public FileInterface {
public:
    DocFile() = default;
    ~DocFile() override { close(); }

    expected<void, std::string> open(const std::string& file_path) override;
    void close() override;
    expected<std::string, std::string> read() override;
    expected<std::string, std::string> read(const std::string& file_path) override;

private:
    std::string __file_path;
};
