#pragma once

#include "core.h"

std::string tool_memory(std::shared_ptr<ControlUnit> ce_ref, json args) {
    (void)ce_ref;
    std::string answer;
    assert(args.contains("keyword") && "tool_memory must contain the 'keyword' key.");
    assert(args.contains("content") && "tool_memory must contain the 'content' key.");
    assert(args.contains("description") && "tool_memory must contain the 'description' key.");
    std::string keyword = args["keyword"].get<std::string>(),
                description = args["description"].get<std::string>(),
                content = args["content"].get<std::string>();
    /*ce_ref->short_term_memory = remove_element_by_name(ce_ref->short_term_memory, "keyword", keyword);
    ce_ref->short_term_memory.push_back({
        { "keyword", keyword },
        { "description", description },
        { "content", content }
    });*/
    if (debug) {
        print_in_line(CYAN, "[memory_data]\t", content);
        print_in_line(CYAN, "[memory_result]\t", "Content has been memorised");
    }
    answer = fmt::format("The content: '{}' has been memorised.", content);
    return answer;
}

std::string tool_send_message(std::shared_ptr<ControlUnit> ce_ref, json args) {
    (void)ce_ref;
    std::string answer;
    if (args.contains("message")) {
        if (!debug) {
            /*std::string completion = ce_ref->agent_executor_state["output"];
            completion = string_in_line(completion);
            completion += "\n";
            stop_spinner(completion);*/
        }
        std::string message = args["message"];
        std::cout << GREEN << "[message] " << RESET << message << "\n";
        answer = fmt::format(
            "The message: '{}' was successfully displayed",
            message
        );
    }
    return answer;
}

std::string tool_user_input(std::shared_ptr<ControlUnit> ce_ref, json args) {
    (void)ce_ref;
    std::string answer;
    if (args.contains("prompt")) {
        if (!debug) {
            /*std::string completion = ce_ref->agent_executor_state["output"].get<std::string>();
            completion = string_in_line(completion);
            completion += "\n";
            stop_spinner(completion);*/
        }
        std::string prompt = args["prompt"];
        std::cout << CYAN << "[question] " << RESET << prompt << "\n> ";
        std::string message = user_input();
        answer = fmt::format(
            "User message is: '{}'",
            message
        );
    }
    return answer;
}

std::string tool_read_file(std::shared_ptr<ControlUnit> ce_ref, json args) {
    (void)ce_ref;
    std::string answer;
    if (args.contains("file_path")) {
        std::string file_path = args["file_path"];
        std::string file_content = read_file(file_path);
        if (debug) {
            print_in_line(CYAN, "[read_file_path]\t", file_path); 
            print_in_line(CYAN, "[read_file_content]\t", file_content);
        }
        answer = fmt::format(
            "The file: '{}' "
            "has been read with content: '{}'", 
            file_path,
            file_content
        );
    }
    return answer;
}

std::string tool_write_file(std::shared_ptr<ControlUnit> ce_ref, json args) {
    (void)ce_ref;
    std::string answer;
    if (args.contains("file_path")) {
        std::string file_path = args["file_path"];
        std::string file_content = args["content"];
        if (debug) {
            print_in_line(CYAN, "[write_file_path]\t", file_path); 
            print_in_line(CYAN, "[write_file_content]\t", file_content);
        }
        write_file(file_path, file_content);
        answer = fmt::format(
            "The content: '{}' "
            "was written to the file: '{}'", 
            file_content,
            file_path
        );
    }
    return answer;
}

std::string tool_append_file(std::shared_ptr<ControlUnit> ce_ref, json args) {
    (void)ce_ref;
    std::string answer;
    if (args.contains("file_path")) {
        std::string file_path = args["file_path"];
        std::string file_content = args["content"];
        if (debug) {
            print_in_line(CYAN, "[append_file_path]\t", file_path); 
            print_in_line(CYAN, "[append_file_content]\t", file_content);
        }
        append_file(file_path, file_content);
        answer = fmt::format(
            "The content: '{}' "
            "was appended to the file: '{}'", 
            file_content,
            file_path
        );
    }
    return answer;
}

std::string tool_execute_bash_command(std::shared_ptr<ControlUnit> ce_ref, json args) {
    (void)ce_ref;
    std::string answer;
    if (args.contains("command")) {
        std::string command = std::string(args["command"]);
        std::string stdout = execute_command(command);
        if (stdout.empty()) {
            stdout = "Success";
        }
        if (debug) {
            print_in_line(CYAN, "[bash_command]\t", command); 
            print_in_line(CYAN, "[bash_result]\t", stdout);
        }
        answer = fmt::format(
            "The bash command: '{}' "
            "was executed with result: '{}'", 
            command,
            stdout
        );
    }
    return answer;
}

std::string tool_execute_python_script(std::shared_ptr<ControlUnit> ce_ref, json args) {
    (void)ce_ref;
    std::string answer;
    if (args.contains("script")) {
        std::string script = std::string(args["script"]);
        std::string dependencies = "";
        if (args.contains("dependencies")) {
            dependencies = std::string(args["dependencies"]);
        }
        /*std::string result = ce_ref->code_interpreter.run_python_code(script, dependencies);
        if (debug) {
            print_in_line(CYAN, "[python_script]\t", script);
            std::cout << CYAN << "[python_result]\t" << RESET << result << "\n";
            //print_in_line(CYAN, "[python_result]\t", result);
        }
        answer = fmt::format(
            "The python script: '{}' "
            "was executed with result: '{}'", 
            script,
            result
        );*/
    }
    return answer;
}

/// TODO: Tree tool
/*
tree<std::string> t;
t.set_head("one");
auto i2 = t.insert(t.end(), "two");
auto i3 = t.insert(t.end(), "three");
t.append_child(i2, "apple");
t.append_child(i2, "pear");   
t.append_child(i3, "banana");
t.append_child(i3, "kiwi");  

print_tree(t);

tree<std::string>::sibling_iterator it=t.begin();
while(static_cast<tree<std::string>::pre_order_iterator>(it) != t.end()) {
    std::cerr << (*it++) << std::endl;
}*/
