#pragma once

#include "core.h"
#include "logger.h"
#include "liboai.h"
#include "context.h"

///
/// @brief LLMClient
///
class LLMClient final : private liboai::Network {
    liboai::OpenAI oai;
    std::string llm_model;
    liboai::Authorization& auth = liboai::Authorization::Authorizer();

    Logger* logger = Logger::get_instance();

public:

	LLMClient() = default;
    ~LLMClient() = default;
    LLMClient(const LLMClient&) = delete;
    LLMClient(LLMClient&&) = delete;

    LLMClient& operator=(const LLMClient&) = delete;
    LLMClient& operator=(LLMClient&&) = delete;

    void set_model(std::string model) {
        llm_model = model;
    }

    void set_provider(std::string endpoint, std::string key) {
        bool result = oai.auth.SetKey(key);
        auth.SetKey(key);
        if (!result) {
            throw std::runtime_error("Failed to set API key");
        }
        oai.ChatCompletion->SetEndpoint(endpoint);
        this->endpoint_root_ = endpoint;
    }

    liboai::Response chat_completion_new(
        const Context& ctx, 
        std::optional<float> temperature, 
        std::optional<float> top_p, 
        std::optional<uint16_t> n, 
        std::optional<std::function<bool(std::string, intptr_t)>> stream, 
        std::optional<std::vector<std::string>> stop, 
        std::optional<uint16_t> max_tokens, 
        std::optional<float> presence_penalty, 
        std::optional<float> frequency_penalty, 
        std::optional<std::unordered_map<std::string, int8_t>> logit_bias, 
        std::optional<std::string> user
    ) const& noexcept(false) {
        liboai::JsonConstructor jcon;
        jcon.push_back("model", llm_model);
        jcon.push_back("temperature", std::move(temperature));
        jcon.push_back("top_p", std::move(top_p));
        jcon.push_back("n", std::move(n));
        jcon.push_back("stream", stream);
        jcon.push_back("stop", std::move(stop));
        jcon.push_back("max_tokens", std::move(max_tokens));
        jcon.push_back("presence_penalty", std::move(presence_penalty));
        jcon.push_back("frequency_penalty", std::move(frequency_penalty));
        jcon.push_back("logit_bias", std::move(logit_bias));
        jcon.push_back("user", std::move(user));
        if (!ctx.empty()) {
            jcon.push_back("messages", ctx);
        }
        auth.SetMaxTimeout(120000); /// ms
        liboai::Response res;
        res = this->Request(
            Method::HTTP_POST, this->endpoint_root_, "/chat/completions", "application/json",
            this->auth.GetAuthorizationHeaders(),
            liboai::netimpl::components::Body {
                jcon.dump()
            },
            stream ? liboai::netimpl::components::WriteCallback{std::move(stream.value())} : liboai::netimpl::components::WriteCallback{},
            this->auth.GetProxies(),
            this->auth.GetProxyAuth(),
            this->auth.GetMaxTimeout()
        );
        logger->log("Response");
        logger->log(json::parse(res.content).dump(4));
        return res;
    }

    liboai::Response chat_completion(liboai::Conversation& conversation, float temperature) {
        guard("LLMClient::chat_completion")
        logger->log("Call chat_completion");
        logger->log(conversation.GetJSON().dump(4));
        oai.auth.SetMaxTimeout(120000); /// ms
        liboai::Response response = oai.ChatCompletion->create(
            llm_model,              /// model
            conversation,           /// conversation
            temperature,            /// temperature
            std::nullopt,           /// top_p
            std::nullopt,           /// n
            std::nullopt,           /// stream
            std::vector<std::string>{"<<CALL>>"}, /// stop
            std::nullopt,           /// max_tokens
            std::nullopt,           /// presence_penalty
            std::nullopt,           /// frequency_penalty
            std::nullopt,           /// logit_bias
            std::nullopt            /// user
        );
        response = liboai::Response(response);
        logger->log("Response");
        logger->log(json::parse(response.content).dump(4));
        return response;
        unguard()
        return liboai::Response();
    }

    liboai::Response embedding(const std::string& text, embedding_model model = embedding_model::oai_3small) {
        guard("LLMClient::embedding")
        liboai::Response response = oai.Embedding->create(
            fmt::format("{}", model),
            text
        );
        //json jres = response["data"][0]["embedding"];
        //std::cout << response << "\n\n";
        return response; //vdb::vector({ jres.begin(), jres.end() }, model);
        unguard()
        return liboai::Response();
    }

    std::future<liboai::Response> embedding_async(const std::string& text, embedding_model model = embedding_model::oai_3small) {
        return std::async(std::launch::async, &LLMClient::embedding, this, text, model);
    }

    /// TODO: Refine
    std::string rag(std::string input, std::string data) {
        std::string result;
        liboai::Conversation working_memory = liboai::Conversation();
        working_memory.SetSystemData("Using QUERY and RESULT output human readable answer.\n\n"
            "QUERY: " + input + 
            "\n\nRESULT: " + data
        );
        liboai::Response response = chat_completion(working_memory, 0.5);
        json content = json::parse(std::string(response.content));
        if (content.contains("choices")) { /// Top level, several messages
            for (auto& choice : content["choices"].items()) {
                if (choice.value().contains("message")) {
                    json message = choice.value()["message"];
                    result = std::string(message["content"]);
                }
            }
        }
        return result;
    }

    /// TODO: Refine
    std::string enrich_json_answer(std::string json_content) {
        std::string result;
        liboai::Conversation working_memory = liboai::Conversation();
        working_memory.SetSystemData("Enrich json object to readable answer.\n"
            "Do not output any additional reasoning other than the enriched result.\n"
            "Below is json content to enrich:\n\n" + 
            json_content
        );
        liboai::Response response = chat_completion(working_memory, 0.5);
        json content = json::parse(std::string(response.content));
        if (content.contains("choices")) { /// Top level, several messages
            for (auto& choice : content["choices"].items()) {
                if (choice.value().contains("message")) {
                    json message = choice.value()["message"];
                    result = std::string(message["content"]);
                }
            }
        }
        return result;
    }

};
