#pragma once

#include "core.h"
#include "doc_types/txt_file.h"
#include "doc_types/pdf_file.h"
#include "doc_types/doc_file.h"
#include "file_helpers.h"

class DocFactory {
public:
    std::unique_ptr<FileInterface> get_instance(const std::string &file_path) {
        std::string extension = FileHelpers::get_file_extension(file_path);
        if (is_plain_text_file(file_path)) {
            return std::make_unique<TxtFile>(file_path);
        } else if (extension == "pdf") {
            return std::make_unique<PdfFile>(file_path);
        } else if (extension == "doc" || extension == "docx") {
            return std::make_unique<DocFile>(file_path);
        } else {
            throw std::runtime_error("Unsupported file type");
        }
    }

private:
    bool is_plain_text_file(const std::string& file_path) {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) { return false; }
        char c;
        while (file.get(c)) {
            if (!isprint(c) && !isspace(c)) { return false; }
        }
        return true;
    }

};
