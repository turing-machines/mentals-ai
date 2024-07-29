#include "../include/logger.h"

/// Initialize static members
std::unique_ptr<Logger> Logger::instance = nullptr;
std::mutex Logger::mutex;

Logger::Logger(const std::string& directory) {
    std::filesystem::create_directory(directory);  /// Ensure directory exists
    std::string filename = generate_filename(directory);
    logfile.open(filename, std::ios::app);
    if (!logfile.is_open()) {
        throw std::runtime_error("Unable to open log file: " + filename);
    }
}

/// Function to generate filename with timestamp
std::string Logger::generate_filename(const std::string& directory) {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << directory << "/log_";
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
    ss << ".txt";
    return ss.str();
}

Logger::~Logger() {
    if (logfile.is_open()) {
        logfile.close();
    }
}

void Logger::log(const std::string& message) {
    logfile << message << std::endl;
    logfile.flush();
}

