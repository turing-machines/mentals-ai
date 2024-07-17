#pragma once

#include "core.h"


class FileHelpers {
public:
    static bool file_exists(const std::string& file_path) {
        return fs::exists(file_path);
    }

    static std::string get_file_extension(const std::string &path) {
        size_t dot_pos = path.find_last_of('.');
        if (dot_pos != std::string::npos) {
            return path.substr(dot_pos + 1);
        }
        return "";
    }

    static std::string get_formatted_time(std::filesystem::file_time_type file_time) {
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
