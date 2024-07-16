#pragma once

#include "core.h"

class FileInterface {
public:
    explicit FileInterface(const std::string& file_path) : __file_path(file_path) {}
    virtual ~FileInterface() = default;

    virtual expected<void, std::string> open() = 0;
    virtual void close() = 0;
    virtual expected<std::string, std::string> read() = 0;
    virtual expected<std::string, std::string> read(const std::string& file_path) = 0;

protected:
    std::string __file_path;

};
