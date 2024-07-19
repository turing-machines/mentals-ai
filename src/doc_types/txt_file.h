#pragma once

#include "core.h"
#include "file_interface.h"

class TxtFile : public FileInterface {
public:
    explicit TxtFile(const std::string& file_path) : FileInterface(file_path) {}
    ~TxtFile() override { close(); }

    expected<void, std::string> open() override;
    void close() override;
    expected<StringBuffer, std::string> read() override;
    expected<StringBuffer, std::string> read(const std::string& file_path) override;

private:
    std::ifstream __file_stream;
    bool is_open = false;

};
