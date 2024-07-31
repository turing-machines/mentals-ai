#include "../include/code_interpreter.h"

CodeInterpreter::CodeInterpreter() {
    python_executable = find_python_executable();
    env_name = "python_env";
#ifdef _WIN32
    env_path = env_name + "\\Scripts";
#else
    env_path = env_name + "/bin";
#endif
}

CodeInterpreter::~CodeInterpreter() {
    delete_virtual_environment();
}

bool CodeInterpreter::create_virtual_environment() {
    if (python_executable.empty()) {
        return false;
    }
    std::string command = python_executable + " -m venv " + env_name + " 2>/dev/null";
    int result = system(command.c_str());
    if (result != 0) {
        //std::cerr << "Error: Failed to create virtual environment" << std::endl;
        return false;
    }
    return true;
}

void CodeInterpreter::delete_virtual_environment() {
    std::string command;
#ifdef _WIN32
    command = "rmdir /s /q " + env_name;
#else
    command = "rm -rf " + env_name;
#endif
    system(command.c_str());
}

std::string CodeInterpreter::run_python_code(const std::string& code, const std::string& dependencies) {
    std::string result;
    ///guard("CodeInterpreter::run_python_code")
    if (python_executable.empty()) {
        return "Python executable not found.";
    }
    std::string error_message;
    if (!dependencies.empty()) {
        error_message = install_dependencies(dependencies);
        if (!error_message.empty()) {
            return "Failed to install dependencies: " + error_message;
        }
    }
    std::string temp_file_name = "temp_code.py";
    std::ofstream out(temp_file_name);
    if (!out) {
        return "Failed to create temporary file.";
    }
    out << code;
    out.close();
    ///
    std::string command = get_activate_command() + " && " + python_executable + " " + temp_file_name + " 2>&1"; // Capture stderr also
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) return "Failed to run Python script.";
    ///
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    ///
    remove(temp_file_name.c_str());
    ///unguard()
    return result;
}

std::string CodeInterpreter::install_dependencies(const std::string& dependencies) {
    std::string command = get_activate_command() + " && " + python_executable + " -m pip install " + dependencies + " 2>&1"; // Capture stderr also
    std::array<char, 128> buffer;
    std::string error_message;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) return "Failed to open pipe for command execution.";
    ///
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        error_message += buffer.data();
    }
    ///
    if (error_message.find("ERROR") != std::string::npos) {
        return error_message;
    }
    return ""; 
}

std::string CodeInterpreter::find_python_executable() const {
    std::array<std::string, 2> versions = {"python3", "python"};
    for (const std::string& version : versions) {
        std::string command = version + " --version";
        if (system((command + " > /dev/null 2>&1").c_str()) == 0) {
            return version;
        }
    }
    return "";
}

std::string CodeInterpreter::get_activate_command() const {
#ifdef _WIN32
    return env_path + "\\activate";
#else
    return "bash -c \"source " + env_path + "/activate\"";
#endif
}
