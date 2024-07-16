#pragma once

#include "core.h"
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
                item_info["created"] = get_formatted_time(entry.last_write_time());
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

    bool file_exists(const std::string& file_path) {
        return fs::exists(file_path);
    }

    std::string read_file(const std::string& file_path) {
        guard("FileManager::read_file")
        DocFactory doc_factory;
        auto file_instance = doc_factory.get_instance(file_path);
        auto result = file_instance->read();
        if (result) { return result.value(); }
        return "";
        unguard()
        return "Error";

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

private:
    std::string get_formatted_time(std::filesystem::file_time_type file_time) {
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            file_time - std::filesystem::file_time_type::clock::now()
            + std::chrono::system_clock::now());
        std::time_t time = std::chrono::system_clock::to_time_t(sctp);
        std::tm tm = *std::localtime(&time);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

};
