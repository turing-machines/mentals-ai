#pragma once

#include "file_interface.h"
#include <poppler-document.h>
#include <poppler-page.h>

class PdfFile : public FileInterface {
public:
    explicit PdfFile(const std::string& file_path) : FileInterface(file_path) {}
    ~PdfFile() override { close(); }

    expected<void, std::string> open() override;
    void close() override;
    expected<StringBuffer, std::string> read() override;
    expected<StringBuffer, std::string> read(const std::string& file_path) override;

private:
    std::unique_ptr<poppler::document> __document;

};
