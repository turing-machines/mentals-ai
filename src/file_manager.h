#pragma once

#include "core.h"
#include "file_helpers.h"
#include "doc_factory.h"

class FileManager {
public:
    class DirectoryIterator {
    public:
        using pointer = const fs::directory_entry*;
        using reference = const fs::directory_entry&;

        DirectoryIterator() : __it(), __is_end(true) {}
        DirectoryIterator(const std::string& dir_path) : __it(fs::directory_iterator(dir_path)), __is_end(false) {
            if (__it == fs::end(__it)) __is_end = true;
        }
        DirectoryIterator(const fs::directory_iterator& it, bool is_end = false) : __it(it), __is_end(is_end) {}

        reference operator*() const { return *__it; }
        pointer operator->() const { return &(*__it); }

        DirectoryIterator& operator++() {
            if (!__is_end) {
                ++__it;
                if (__it == fs::end(__it)) __is_end = true;
            }
            return *this;
        }

        DirectoryIterator operator++(int) {
            DirectoryIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const DirectoryIterator& a, const DirectoryIterator& b) {
            return a.__it == b.__it && a.__is_end == b.__is_end;
        }

        friend bool operator!=(const DirectoryIterator& a, const DirectoryIterator& b) {
            return !(a == b);
        }

    private:
        fs::directory_iterator __it;
        bool __is_end;
    };

    DirectoryIterator begin(const std::string& dir_path) {
        guard("DirectoryIterator::begin")
        if (fs::exists(dir_path) && fs::is_directory(dir_path)) {
            return DirectoryIterator(dir_path);
        }
        unguard_fs()
        return DirectoryIterator();
    }

    DirectoryIterator end() {
        return DirectoryIterator(fs::directory_iterator(), true);
    }

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
            for (auto it = begin(dir_path); it != end(); ++it) {
                const auto& entry = *it;
                json item_info;
                item_info["path"] = entry.path().string();
                item_info["name"] = entry.path().filename().string();
                item_info["extension"] = FileHelpers::get_file_extension(entry.path().filename());
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

    expected<StringBuffer, std::string> read_file(const std::string& file_path) {
        guard("FileManager::read_file")
        if (FileHelpers::exists(file_path)) {
            auto file_instance = DocFactory::get_instance(file_path);
            if (file_instance) {
                auto open_res = file_instance->open();
                if (open_res.has_value()) {
                    auto result = file_instance->read();
                    return result;
                } else {
                    /// Open error
                }
            } else {
                /// Not supported
            }
        } else {
            return unexpected(fmt::format("File '{}' not exist", file_path));
        }
        unguard()
        return unexpected("");
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
