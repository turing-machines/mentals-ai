#pragma once

#include "core.h"

class Logger {
private:
    std::ofstream logfile;
    static std::unique_ptr<Logger> instance;
    static std::mutex mutex;

    Logger(const std::string& directory);

    std::string generate_filename(const std::string& directory);

public:
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
    ~Logger();

    static Logger* get_instance(const std::string& directory = "./logs") {
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = std::unique_ptr<Logger>(new Logger(directory));
        }
        return instance.get();
    }

    void log(const std::string& message);
};

