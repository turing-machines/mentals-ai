#pragma once

#include "core.h"
#include "file.h"

class TxtFile : public FileInterface {
public:
    TxtFile() = default;
    ~TxtFile() override { close(); }

    expected<void, std::string> open(const std::string& file_path) override;
    void close() override;
    expected<std::string, std::string> read() override;
    expected<std::string, std::string> read(const std::string& file_path) override;

private:
    std::ifstream __file_stream;
    bool is_open = false;
};
