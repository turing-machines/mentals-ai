///
/// Experimental implementation of a central executive unit for LLM
/// Turing Machines, Inc.
/// Author: https://x.com/CostaAl4
///

#include "platform.h"
#include "pgvector.h"
#include "agent_executor.h"

bool debug{false};
std::atomic<bool> spinner_active{false};
std::thread spinner_thread;
std::string completion_text;

int main(int argc, char *argv[]) {

    guard("Mentals")

    std::string input, filename = parse_input(argc, argv, input);

/*
    const std::string conn_info = "dbname=memory user=postgres password=postgres hostaddr=127.0.0.1 port=5432";

    try {
        // Establishing a connection to the database
        pqxx::connection c(conn_info);
        if (c.is_open()) {
            std::cout << "Opened database successfully: " << c.dbname() << std::endl;
        } else {
            std::cerr << "Can't open database" << std::endl;
            return 1;
        }

        // Create a transactional object.
        pqxx::work txn(c);

        // Example query to create a table with a pgvector column
        std::string sql = R"(
            CREATE TABLE IF NOT EXISTS items (
                id SERIAL PRIMARY KEY,
                data VECTOR(3) -- Assuming 3D vectors for example
            );
        )";

        txn.exec(sql);
        txn.commit();

        std::cout << "Table created successfully" << std::endl;

        // Example of inserting data into the pgvector column
        pqxx::work txn_insert(c);
        pqxx::result r = txn_insert.exec("INSERT INTO items (data) VALUES ('[1.6, 2.3, 3.8]');");
        txn_insert.commit();

        std::cout << "Data inserted successfully" << std::endl;

        // Example query to select data
        pqxx::nontransaction non_txn(c);
        pqxx::result res = non_txn.exec("SELECT id, data FROM items;");

        std::cout << "Selected data:" << std::endl;
        for (auto row : res) {
            std::cout << "ID = " << row["id"].as<int>() << ", Data = ";
            pgvector::Vector vec = row["data"].as<pgvector::Vector>();

           std::cout << vec;

            std::cout << std::endl;
        }

        c.close();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
*/

    Logger* logger = Logger::get_instance();
    logger->log("Mentals started");

    /// Fetch platfrom info
    std::string platform_info = get_platform_info(false);
    logger->log("\n" + platform_info);

    /// Load config
    auto config     = toml::parse_file("config.toml");
    auto endpoint   = config["llm"]["endpoint"].value_or<std::string>("");
    auto api_key    = config["llm"]["api_key"].value_or<std::string>("");
    auto model      = config["llm"]["model"].value_or<std::string>("");
    auto dbname     = config["vdb"]["dbname"].value_or<std::string>("memory");
    auto user       = config["vdb"]["user"].value_or<std::string>("postgres");
    auto password   = config["vdb"]["password"].value_or<std::string>("postgres");
    auto hostaddr   = config["vdb"]["hostaddr"].value_or<std::string>("127.0.0.1");
    auto port       = config["vdb"]["port"].value_or<std::string>("5432");

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

    /*std::string conn_info = fmt::format("dbname={} user={} password={} hostaddr={} port={}", 
        dbname, user, password, hostaddr, port);

    PgVector db(conn_info);
    db.connect();
    std::cout << db.list_collections() << std::endl;
    db.create_collection("vector_table", EmbeddingModel::ada002);
    std::cout << db.list_collections() << std::endl;
    db.delete_collection("vector_table");
*/

    /// Init central executive
    ///auto agent_executor = std::make_shared<AgentExecutor>(conn);
    auto agent_executor = std::make_shared<AgentExecutor>();

    agent_executor->llm.set_provider(endpoint, api_key);
    agent_executor->llm.set_model(model);
    /// Set central executive state variables
    agent_executor->set_state_variable("current_date", get_current_date());
    agent_executor->set_state_variable("platform_info", platform_info);

    /// Init native tools
    if (!agent_executor->init_native_tools("native_tools.toml")) {
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
    agent_executor->init_agent(instructions);

    /// Run agent from root instruction
    agent_executor->run_agent_thread("root", input);

    /// Final stat
    fmt::print(
        "{}--------------------------------------------\n"
        "Tok/s: {} (completion tokens / total time)\n"
        "Completion tokens: {}\n"
        "Total tokens: {}\n"
        "Total NLOP: {}\n"
        "NLOPS: {:.2f}\n",
        RESET, agent_executor->toks,
        agent_executor->usage["completion_tokens"].get<int>(),
        agent_executor->usage["total_tokens"].get<int>(),
        agent_executor->nlop, agent_executor->nlops
    );

    exit(EXIT_SUCCESS);

    unguard()
}
