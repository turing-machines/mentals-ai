#pragma once

#include "core.h"
#include "file_helpers.h"
#include "doc_factory.h"

class FileManager {
public:
    bool create_directory(const std::string& dir_path) {
        guard("FileManager::create_directory")
        return fs::create_directory(dir_path);
        unguard_fs()
        return false;
    }

    bool delete_directory(const std::string& dir_path) {
        guard("FileManager::delete_directory")
        return fs::remove_all(dir_path) > 0;
        unguard_fs()
        return false;
    }

    json list_directory(const std::string& dir_path) {
        guard("FileManager::list_directory")
        json items = json::array();
        if (fs::exists(dir_path) && fs::is_directory(dir_path)) {
            for (const auto& entry : fs::directory_iterator(dir_path)) {
                json item_info;
                item_info["path"] = entry.path().string();
                item_info["name"] = entry.path().filename().string();
                item_info["size"] = entry.is_regular_file() ? fs::file_size(entry.path()) : 0;
                item_info["created"] = FileHelpers::get_formatted_time(entry.last_write_time());
                item_info["is_directory"] = entry.is_directory();
                items.push_back(item_info);
            }
        }
        return items;
        unguard_fs()
        return json::array();
    }

    bool create_file(const std::string& file_path) {
        std::ofstream file(file_path);
        if (file) { return true; } else { return false; }
    }

    bool delete_file(const std::string& file_path) {
        guard("FileManager::delete_file")
        return fs::remove(file_path);
        unguard_fs()
        return false;
    }

    std::string read_file(const std::string& file_path) {
        guard("FileManager::read_file")
        if (FileHelpers::file_exists(file_path)) {
            DocFactory doc_factory;
            auto file_instance = doc_factory.get_instance(file_path);
            auto open_res = file_instance->open();
            if (open_res.has_value()) {
                auto result = file_instance->read();
                if (result.has_value()) { return result.value(); }
                else {
                    /// Read error
                }
            } else {
                /// Open error
            }
        } else {
            return fmt::format("File {} not exist", file_path);
        }
        unguard()
        return "Exception";

    }

    /// TODO: DocFactory depending on file type
    bool write_file(const std::string& file_path, const std::string& content) {
        std::ofstream file(file_path);
        if (file) {
            file << content;
            return true;
        } else { 
            return false; 
        }
    }

};
