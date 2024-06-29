#pragma once

#include "file.h"
#include <poppler-document.h>
#include <poppler-page.h>

class PdfFile : public FileInterface {
public:
    PdfFile() = default;
    ~PdfFile() override { close(); }

    expected<void, std::string> open(const std::string& file_path) override;
    void close() override;
    expected<std::string, std::string> read() override;
    expected<std::string, std::string> read(const std::string& file_path) override;

private:
    std::unique_ptr<poppler::document> __document;
};
