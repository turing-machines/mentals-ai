///
/// Experimental implementation of a central executive unit for LLMClient
/// Turing Machines, Inc.
/// Author: https://x.com/CostaAl4
///

#include "cli/cli11.hpp"

#include "platform.h"
#include "pgvector.h"
#include "context.h"
#include "memory_controller.h"
#include "control_unit.h"
#include "file_manager.h"
#include "pipeline.h"
#include "data_transfer.h"
#include "gen_file.h"
#include "web_server.h"
///#include "terminal_chat.h"
#include "tui/progress_bar.h"


bool debug{false};
std::atomic<bool> spinner_active{false};
std::thread spinner_thread;
std::string completion_text;


int main(int argc, char *argv[]) {

    guard("Mentals")

    CLI::App app{"Mentals - Central Executive Unit for LLM"};

    bool list_collections = false;
    std::string input, collection, path, toolfile;
    app.add_option("-p,--path", path, "File or directory path");
    app.add_option("-c,--collection", collection, "Collection name");
    app.add_option("-i,--input", input, "Input string");
    app.add_option("-t,--tools-write", toolfile, "Write tools from TOML file to memory");
    app.add_flag("-l,--list-collections", list_collections, "List collections");
    app.add_flag("-d,--debug", debug, "Enable debug mode");

    CLI11_PARSE(app, argc, argv);

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

    //WebServer mentals_chat(DEFAULT_ADDRESS, 9002, 8080);
    //mentals_chat.start();

    auto llm = std::make_shared<LLMClient>();
    llm->set_provider(endpoint, api_key);
    llm->set_model(model);

    std::shared_ptr<EmbeddingsInterface> emb = std::make_shared<EmbeddingsProvider>();
    emb->set_provider(endpoint, api_key);
    emb->set_model(embedding_model::oai_3small);

    std::string conn_info = fmt::format("dbname={} user={} password={} hostaddr={} port={}", 
        dbname, user, password, hostaddr, port);

    std::shared_ptr<PgVector> vdb = std::make_shared<PgVector>(conn_info);
    vdb->connect();

    auto memc = std::make_shared<MemoryController>(emb, vdb);

 /*   memc.delete_collection("books");
    memc.create_collection("books");

    std::vector<std::string> file_paths = {
        "assets/thus_spoke_zarathustra.pdf"
        //"assets/15_the_spirit_in_man_art_and_literature.pdf",
        //"assets/psychology_and_religion.pdf"
    };

    std::shared_ptr<FileInterface> file = std::make_shared<PdfFile>();
    for (const auto& file_path : file_paths) {
        fmt::print("{}Loading file: {}{}\n", YELLOW, file_path, RESET);
        auto file_res = file->read(file_path);
        if (!file_res) {
            std::cerr << file_res.error() << std::endl;
            continue;
        }
        fmt::print("{} chunking...\n", file_path);
        std::vector<std::string> chunks = split_text_by_sentences(file_res.value(), 20);
        std::string file_name = get_filename_w_ext(file_path);
        memc.process_chunks(chunks, file_name);
    }

    memc.write_chunks("books");

    std::string query = "What does he thinks about life?";

    /// Read chunks
    auto chunks_res = memc.read_chunks("books", query, 5);

    if (chunks_res) {

        json j_chunks = json::array();
        for (const auto& chunk : *chunks_res) {
            j_chunks.push_back(chunk.serialize_json());
        }

        std::cout << "Search results:\n" << j_chunks.dump(4) << "\n\n"; 

        liboai::Conversation conv;
        conv.SetSystemData(
            "You are a helpful assistant.\n"
            "Answer the user's question in no more than 300 words.\n"
            "Use the content below to answer the user's question.\n"
            "At the end of your answer, cite references in the format `#content_id#chunk_id` for chunks in memory.\n\n"
            "----- CONTENT SECTION -----\n" +
            (*chunks_res).dump(4)
        );
        conv.AddUserData(query, "user");
        liboai::Response response = llm.chat_completion(conv, 0.5);
        json content = json::parse(std::string(response.content));
        if (content.contains("choices")) {
            for (auto& choice : content["choices"].items()) {
                if (choice.value().contains("message")) {
                    json message = choice.value()["message"];
                    std::string content = std::string(message["content"]);
                    if (!content.empty()) {
                        fmt::print("\n-----\n{}\n\n", content);
                    }
                }
            }
        }
    }
*/
    if (list_collections) {

        auto lc = vdb->list_collections();
        if (lc.has_value()) {
            fmt::print("Collections list\n{}", (*lc).dump(4));
        }

    } else if (!path.empty() && !collection.empty()) {

        ProgressBar progress_bar("Transfer", "Complete");
        auto progress_callback = [&progress_bar](double progress) {
            progress_bar.update(static_cast<float>(progress));
        };
        memc->set_progress_callback(progress_callback);
    
        auto fmgr = std::make_shared<FileManager>();


        PipelineFactory factory;
        factory.register_stage_with_args<FileReaderToStringBuffer>("FileReaderToStringBuffer", fmgr);
        factory.register_stage_with_args<ChunkBufferToMemoryController<std::string>>("ChunkBufferToMemoryController", memc);
        factory.register_stage<StringBufferToChunkBuffer<std::string>>("StringBufferToChunkBuffer");
        factory.register_stage<ChunkBufferToPrint<std::string>>("ChunkBufferToPrint");


        Pipeline pipeline(factory);
        pipeline.add_stage("FileReaderToStringBuffer");
        pipeline.add_stage("StringBufferToChunkBuffer");
        //pipeline.add_stage("ChunkBufferToPrint");
        pipeline.add_stage("ChunkBufferToMemoryController");

        /*pipeline.async_result_handler([](
            const std::string& input,
            std::shared_ptr<SafeChunkBuffer<std::string>> result
        ) {
            if (result) {
                ///fmt::print("Filename: {}\nChunk count: {}\n\n", input, result->get_chunks_count());
            }
        });*/


        /// TOFIX:
        /// Exception in EmbeddingsProvider::embeddings: General exception: 
        /// liboai::netimpl::CurlHolder::CurlHolder(): Failed initialization (E_CURLERROR:0x06)
        /// Exception in MemoryController::chunk_embeddings: General exception: 
        /// liboai::netimpl::CurlHolder::CurlHolder(): Failed initialization (E_CURLERROR:0x06)

        auto p_input = std::make_shared<std::string>(path);
        auto result = pipeline.execute(p_input);

        //memc->process_chunks({ "test" }, "test");

        ///pipeline.execute_async(path);
        ///pipeline.lfg_async(collection);

        ///if (result) {
        ///    std::cout << "Res:\n---\n" << result->get_data()[0] << "\n\n";
        ///}

        /// FileManager -> MemoryController
        ///DataTransfer<FileManager, MemoryController> fs2memory(fmgr, memc);
        ///if (FileHelpers::is_file(path)) {
        ///    fs2memory.transfer(path, collection);
        ///} else if (FileHelpers::is_directory(path)) {
        ///    fs2memory.bulk_transfer(path, collection);
        ///}

    }

/*
    ///json ls = fmgr->list_directory(input);
    ///fmt::print("{}\n\n", ls.dump(4));
    //std::string file_content = fmgr.read_file(input);
    //fmt::print("{}\n\n", file_content);
    /*for (auto it = fmgr.begin(input); it != fmgr.end(); ++it) {
        const auto& entry = *it;
        std::cout << entry.path() << std::endl;
    }*/


 /*   if (!toolfile.empty()) {

        std::cout << toolfile;

        memc->delete_collection("tools");
        memc->create_collection("tools");

        ProgressBar progress_bar("Updating tools:", "Complete");
        auto progress_callback = [&progress_bar](double progress) {
            progress_bar.update(static_cast<float>(progress));
        };

        memc->set_progress_callback(progress_callback);

        auto native_instructions_toml = toml::parse_file(toolfile);
        auto tools = native_instructions_toml["instruction"].as_array();

        std::stringstream ss;
        ss << toml::json_formatter{ *tools };
        json native_instructions = json::parse(ss.str());

        for (auto& item : native_instructions) {
            std::string tool_text;
            //tool_text = item.dump(4);
            //tool_text = std::string(item["name"]);
            tool_text = item.dump(4);
            //tool_text = std::string(item["description"]);
            //std::cout << tool_text << "\n-------\n";
            std::vector<std::string> chunks = { tool_text };
            memc->process_chunks(chunks, item["name"]);
        }

        memc->write_chunks("tools");

    } else if (path.empty() && !input.empty()) {

        memc->create_collection("messages");

        auto ctrl = std::make_shared<ControlUnit>(llm, memc);
        ctrl->init();
        ctrl->load_latest_messages(5);

        ctrl->process_request(input);

        ///TerminalChat chat_app;
        ///chat_app.run();

    } else if (!path.empty()) {

        memc->create_collection("messages");

        GenFile gen;
        auto [variables, instructions] = gen.load_from_file(path);
        std::string user_input = instructions["root"].prompt;

        ControlUnit ctrlu(std::move(llm), std::move(memc));
        ctrlu.init();

        ctrlu.process_request(user_input);
    }

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
    auto [variables, instructions] = gen.load_from_file(path);

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
