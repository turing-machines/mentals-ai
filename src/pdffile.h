#pragma once

#include "file.h"
#include <poppler-document.h>
#include <poppler-page.h>

class PdfFile : public FileInterface {
public:
    PdfFile() = default;
    ~PdfFile() override { close(); }

    expected<void, std::string> open(const std::string &file_path) override;
    expected<std::string, std::string> read() override;
    void close() override;

private:
    std::unique_ptr<poppler::document> __document;
};
