#pragma once

#include "core.h"

class FileInterface {
public:
    virtual ~FileInterface() = default;

    virtual expected<void, std::string> open(const std::string &file_path) = 0;
    virtual void close() = 0;
    virtual expected<std::string, std::string> read() = 0;
    virtual expected<std::string, std::string> read(const std::string& file_path) = 0;
};
