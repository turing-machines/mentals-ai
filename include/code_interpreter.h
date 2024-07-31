#pragma once

#include "core.h"

class CodeInterpreter {
public:
    CodeInterpreter();
    ~CodeInterpreter();
    /// 
    bool create_virtual_environment();
    void delete_virtual_environment();
    std::string run_python_code(const std::string& code, const std::string& dependencies = "");

private:
    std::string env_name;
    std::string env_path;
    std::string python_executable;
    ///
    std::string get_activate_command() const;
    std::string install_dependencies(const std::string& dependencies);
    std::string find_python_executable() const;
};
