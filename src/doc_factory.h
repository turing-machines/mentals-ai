#pragma once

#include <unordered_set>

#include "core.h"
#include "doc_types/txt_file.h"
#include "doc_types/pdf_file.h"
#include "doc_types/doc_file.h"
#include "file_helpers.h"

class DocFactory {
public:
    static std::unique_ptr<FileInterface> get_instance(const std::string &file_path) {
        auto extension = is_supported(file_path);
        if (!extension) { return nullptr; }
        if (extension == "plain_text") {
            return std::make_unique<TxtFile>(file_path);
        } else if (extension == "pdf") {
            return std::make_unique<PdfFile>(file_path);
        } else if (extension == "doc" || extension == "docx") {
            return std::make_unique<DocFile>(file_path);
        }
        return nullptr;
    }

    static std::optional<std::string> is_supported(const std::string &file_path) {
        std::string extension = FileHelpers::get_file_extension(file_path);
        if (is_plain_text_file(file_path)) { return "plain_text"; }
        static const std::unordered_set<std::string> supported_formats = { "pdf", "doc", "docx" };
        if (supported_formats.find(extension) != supported_formats.end()) { return extension; }
        return std::nullopt;
    }

private:
    static bool is_plain_text_file(const std::string& file_path) {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) { return false; }
        char c;
        while (file.get(c)) {
            if (!isprint(c) && !isspace(c)) { return false; }
        }
        return true;
    }

};
