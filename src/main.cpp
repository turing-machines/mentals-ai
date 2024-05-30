///
/// Experimental implementation of a central executive unit for LLM
/// Turing Machines, Inc.
/// Author: https://x.com/CostaAl4
///

#include "platform.h"
#include "central_executive.h"

bool debug{false};
std::atomic<bool> spinner_active{false};
std::thread spinner_thread;
std::string completion_text;

int main(int argc, char *argv[]) {

    guard("Mentals")

    std::string input;
    std::string filename = parse_input(argc, argv, input);

    Logger* logger = Logger::get_instance();
    logger->log("Mentals started");

    /// Fetch platfrom info
    std::string platform_info = get_platform_info(false);
    logger->log("\n" + platform_info);

    /// Load config
    auto config             = toml::parse_file("config.toml");
    std::string endpoint    = config["ENDPOINT"].value_or<std::string>("");
    std::string api_key     = config["API_KEY"].value_or<std::string>("");
    std::string model       = config["MODEL"].value_or<std::string>("");
    if (debug) {
        fmt::print(
            "Endpoint:\t{}\n"
            "API key:\t{}\n"
            "Model:\t\t{}\n\n"
            "Platform info\n"
            "-----------------------------\n"
            "{}\n\n",
            endpoint, mask_api_key(api_key), model, platform_info
        );
    }

    /// Init central executive
    auto ce = std::make_shared<CentralExecutive>();
    ce->llm.set_provider(endpoint, api_key);
    ce->llm.set_model(model);
    /// Set central executive state variables
    ce->set_state_variable("current_date", get_current_date());
    ce->set_state_variable("platform_info", platform_info);

    /// Init native tools
    if (!ce->init_native_tools("native_tools.toml")) {
        throw std::runtime_error("Failed to init native tools");
    }

    /// Load agent file
    GenFile gen;
    auto [variables, instructions] = gen.load_from_file(filename);

    /// Render variables
    /// TODO: Move into GenFile class
    variables["input"] = input;
    for (auto& [key, value] : instructions) {
        value.prompt = render_template(value.prompt, variables);
    }

    /// Init agent
    ce->init_agent(instructions);

    /// Run agent from root instruction
    ce->run_agent_thread("root", input);

    /// Final stat
    fmt::print(
        "{}-----------------------------\n"
        "NLOP count: {}\n"
        "Completion tokens: {}\n"
        "Total tokens: {}\n",
        RESET, ce->nlop,
        ce->usage["completion_tokens"].get<int>(),
        ce->usage["total_tokens"].get<int>()
    );

    exit(EXIT_SUCCESS);

    unguard()
}
