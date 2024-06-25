///
/// Experimental implementation of a central executive unit for LLM
/// Turing Machines, Inc.
/// Author: https://x.com/CostaAl4
///

#include "platform.h"
#include "pgvector.h"
#include "context.h"
#include "memory_controller.h"
#include "agent_executor.h"

#include "pdffile.h"

bool debug{false};
std::atomic<bool> spinner_active{false};
std::thread spinner_thread;
std::string completion_text;


int main(int argc, char *argv[]) {

    guard("Mentals")

    std::string input, filename = parse_input(argc, argv, input);

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

    LLM llm;
    llm.set_provider(endpoint, api_key);
    llm.set_model(model);

    std::string conn_info = fmt::format("dbname={} user={} password={} hostaddr={} port={}", 
        dbname, user, password, hostaddr, port);

    PgVector vdb(conn_info);
    vdb.connect();

    EmbeddingModel embed_model = EmbeddingModel::oai_3small;

    vdb.delete_collection("books");
    vdb.create_collection("books", embed_model);

    /// Memory controller routines
    /// 1. Read data from source
    /// 2. Chunking
    /// 3. Embedding in parallel (keep chunk sequence)
    /// 4. Write data to vdb

    std::string file_name = "ThusSpokeZarathustra.pdf";

    /// Step 1: Read data from source
    std::unique_ptr<FileInterface> file = std::make_unique<PdfFile>();
    auto file_res = file->open("assets/" + file_name);
    if (!file_res) {
        std::cerr << file_res.error() << std::endl;
        exit(EXIT_FAILURE);
    }
    auto read_res = file->read();
    if (!read_res) {
        std::cerr << read_res.error() << std::endl;
        exit(EXIT_FAILURE);
    }
    file->close();

    /// TODO: Move to memory controller
    /// Implement MemoryController class

    /// Step 2: Chunking
    std::vector<std::string> chunks = split_text_by_sentences(read_res.value(), 5);

    /// Step 3: Embedding 
    std::vector<std::future<MemChunk>> futures;
    std::string idx = gen_index();
    for (std::size_t i = 0; i < chunks.size(); ++i) {
        std::string chunk_text = chunks[i]; 
        fmt::print("\033[0mmem_chunk #{}: Processing started.\n", i);
        futures.push_back(std::async(std::launch::async, 
            process_chunks_async, std::ref(llm), embed_model, 
            i, idx, chunk_text, file_name, std::nullopt));
    }

    /// Step 4: Write to storage
    auto txn = vdb.create_transaction();
    for (auto& fut : futures) {
        MemChunk res = fut.get();
        vdb.write_content(*txn, "books", res.idx, res.content, res.embedding, res.name, res.meta);
        fmt::print("\033[0mmem_chunk #{}: Data written to memory.\n", res.chunk_id);
    }
    vdb.commit_transaction(txn);
    fmt::print("All chunks have been processed.\n");

    ///
    /// Fetch data
    ///
    std::string search_text = "What he thinks about ego?";
    fmt::print("\nSearch text:\n{}\n\n", search_text);
    vdb::vector search_vec = llm.embedding(search_text, embed_model);
    auto search_res = vdb.search_content("books", search_vec, 5, vdb::QueryType::cosine_similarity);
    if (search_res) {
        std::cout << "Search results:\n" << (*search_res).dump(4) << "\n\n";
    }


/*
    vdb.delete_collection("tools");
    vdb.delete_collection("gens");
    vdb.create_collection("tools", EmbeddingModel::oai_3small);
    ///vdb.create_collection("gens", EmbeddingModel::oai_ada002);
    //vdb.create_collection("tools", EmbeddingModel::oai_3large);

    auto res = vdb.list_collections();

    if (res) {
        std::cout << "Collections: " << *res << "\n\n";
    }

    auto native_instructions_toml = toml::parse_file("native_tools.toml");
    auto tools = native_instructions_toml["instruction"].as_array();

    std::stringstream ss;
    ss << toml::json_formatter{ *tools };
    json native_instructions = json::parse(ss.str());

    for (auto& item : native_instructions) {
        std::string tool_text;
        //tool_text = item.dump(4);
        //tool_text = std::string(item["name"]);
        tool_text = std::string(item["name"]) + "\n" + std::string(item["description"]);
        if (item.contains("parameters")) {
            for (auto& param : item["parameters"]) {
                tool_text += "\n" 
                    + std::string(param["name"]) + " : " 
                    + std::string(param["description"]);
            }
        }
        //tool_text = std::string(item["description"]);
        std::cout << tool_text << "\n-------\n";
        vdb::vector vec = llm.embedding(tool_text); //, EmbeddingModel::oai_3large);
        
        std::string idx = gen_index();

        vdb.write_content("tools", idx, item["name"], vec, item["name"]);
    }

    GenFile gen;
    auto [variables, instructions] = gen.load_from_file(filename);
    std::string search_text = instructions["root"].prompt;
    fmt::print("Search text:\n{}\n\n", search_text);
    fmt::print("Tools: {}\n\n", instructions["root"].use);
    vdb::vector search_vec = llm.embedding(search_text); //, EmbeddingModel::oai_3large);
    auto search_res = vdb.search_content("tools", search_vec, 5, vdb::QueryType::cosine_similarity);
    if (search_res) {
        std::cout << "Search results:\n" << (*search_res).dump(4) << "\n\n";
    }

    auto info = vdb.get_collection_info("tools");
    if (info) {
        std::cout << "tools info: " << (*info).dump(4) << "\n\n";
    }
*/

/*
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
*/

    exit(EXIT_SUCCESS);

    unguard()
}
