#pragma once

#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <stdexcept>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__APPLE__) || defined(__MACH__)
#include <sys/utsname.h>
#elif defined(__linux__)
#include <fstream>
#include <sstream>
#else
std::string get_os_name_and_version() {
    return "Unknown OS";
}
#endif

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string get_os_name_and_version() {
#if defined(_WIN32) || defined(_WIN64)
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionEx(&osvi)) {
        return "Windows " + std::to_string(osvi.dwMajorVersion) + "." + std::to_string(osvi.dwMinorVersion);
    } else {
        return "Unknown Windows version";
    }
#elif defined(__APPLE__) || defined(__MACH__)
    struct utsname buffer;
    if (uname(&buffer) != 0) {
        return "Unknown macOS version";
    }
    return std::string(buffer.sysname) + " " + buffer.release;
#elif defined(__linux__)
    std::ifstream file("/etc/os-release");
    if (!file.is_open()) {
        return "Unknown Linux version";
    }
    std::string line;
    std::string name;
    while (std::getline(file, line)) {
        if (line.find("PRETTY_NAME=") == 0) {
            name = line.substr(13, line.size() - 14); // Remove PRETTY_NAME=" and trailing "
            break;
        }
    }
    if (name.empty()) {
        return "Unknown Linux version";
    }
    return name;
#else
    return "Unknown OS";
#endif
}

std::string get_python_info(const char* python_command, bool include_packages) {
    std::string python_code = 
        "import platform\n"
        "print('Python Version:', platform.python_version())\n";
        //"print('Python Implementation:', platform.python_implementation())\n"
        //"print('Python Compiler:', platform.python_compiler())\n"
        //"print('Python Build:', platform.python_build())\n"
        //"print('Python Platform:', platform.platform())\n";
    ///
    if (include_packages) {
        python_code += 
            "import pkg_resources\n"
            "print('Installed Packages:')\n"
            "for dist in pkg_resources.working_set:\n"
            "    print(dist.project_name, dist.version)\n";
    }
    std::string command = std::string(python_command) + " -c \"" + python_code + "\"";
    return exec(command.c_str());
}

bool command_exists(const std::string& cmd) {
    std::string result = exec(("command -v " + cmd + " 2>&1").c_str());
    return !result.empty();
}

std::string get_package_manager_info() {
    std::string info;
    std::array<const char*, 4> package_managers = {"pip", "conda", "poetry", "pipenv"};
    for (const char* manager : package_managers) {
        if (command_exists(manager)) {
            std::string command = std::string(manager) + " --version";
            try {
                std::string version = exec(command.c_str());
                info += manager;
                info += " version: ";
                info += version;
            } catch (const std::runtime_error&) {
                info += manager;
                info += " version not found.\n";
            }
        } else {
            info += manager;
            info += " not found.\n";
        }
    }
    return info;
}

std::string get_platform_info(bool include_packages) {
    std::string os_info = get_os_name_and_version();
    std::string python_info;
    try {
        python_info = get_python_info("python3", include_packages);
    } catch (const std::runtime_error&) {
        try {
            python_info = get_python_info("python", include_packages);
        } catch (const std::runtime_error&) {
            python_info = "Python not found";
        }
    }
    std::string package_manager_info = get_package_manager_info();
    return "Operating System: " + os_info + "\n" + python_info + "\n" + package_manager_info;
}
