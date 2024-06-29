#pragma once

#include "core.h"
#include "doc_types/txt_file.h"
#include "doc_types/pdf_file.h"
#include "doc_types/doc_file.h"

class FileFactory {
public:
    std::unique_ptr<FileInterface> get_instance(const std::string &file_path) {
        std::string extension = get_file_extension(file_path);
        if (extension == "txt") {
            return std::make_unique<TxtFile>();
        } else if (extension == "pdf") {
            return std::make_unique<PdfFile>();
        } else if (extension == "doc" || extension == "docx") {
            return std::make_unique<DocFile>();
        } else {
            throw std::runtime_error("Unsupported file type");
        }
    }
};
