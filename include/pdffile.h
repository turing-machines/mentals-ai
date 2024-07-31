#pragma once

#include "file.h"
#include "poppler/cpp/poppler-document.h"
#include "poppler/cpp/poppler-page.h"
#include <memory>

class PdfFile : public FileInterface {
public:
    PdfFile() = default;
    ~PdfFile() override { close(); }

    expected<void, std::string> open(const std::string& file_path) override;
    expected<std::string, std::string> read() override;
    void close() override;

    expected<std::string, std::string> read_file(const std::string& file_path) override;

private:
    std::unique_ptr<poppler::document> __document;
};
