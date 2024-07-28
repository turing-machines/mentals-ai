#pragma once

#include "core.h"
#include "logger.h"
#include "liboai.h"
#include "context.h"

using namespace liboai;


///
/// @brief LLMClient
///
class LLMClient final : private Network {
    std::string llm_model;
    Authorization& auth = Authorization::Authorizer();

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
        bool result = auth.SetKey(key);
        if (!result) {
            throw std::runtime_error("Failed to set API key");
        }
        this->endpoint_root_ = endpoint;
    }

    Response chat_completion(
        const Context& ctx, 
        std::optional<float> temperature = DEF_TEMP, 
        std::optional<float> top_p = std::nullopt, 
        std::optional<uint16_t> n = std::nullopt, 
        std::optional<std::function<bool(std::string, intptr_t)>> stream = std::nullopt, 
        std::optional<std::vector<std::string>> stop = std::nullopt, 
        std::optional<uint16_t> max_tokens = std::nullopt, 
        std::optional<float> presence_penalty = std::nullopt, 
        std::optional<float> frequency_penalty = std::nullopt, 
        std::optional<std::unordered_map<std::string, int8_t>> logit_bias = std::nullopt, 
        std::optional<std::string> user = std::nullopt
    ) const& noexcept(false) {
        guard("LLMClient::chat_completion")
        JsonConstructor jcon;
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
        auth.SetMaxTimeout(DEF_TIMEOUT);
        Response response;
        response = this->Request(
            Method::HTTP_POST, this->endpoint_root_, "/chat/completions", "application/json",
            this->auth.GetAuthorizationHeaders(),
            netimpl::components::Body {
                jcon.dump()
            },
            stream ? netimpl::components::WriteCallback{std::move(stream.value())} 
                : netimpl::components::WriteCallback{},
            this->auth.GetProxies(),
            this->auth.GetProxyAuth(),
            this->auth.GetMaxTimeout()
        );
        logger->log("Response");
        logger->log(json::parse(response.content).dump(4));
        return response;
        unguard()
        return Response();
    }

    /// TODO: Refine
    /*std::string rag(std::string input, std::string data) {
        std::string result;
        Conversation working_memory = Conversation();
        working_memory.SetSystemData("Using QUERY and RESULT output human readable answer.\n\n"
            "QUERY: " + input + 
            "\n\nRESULT: " + data
        );
        Response response = chat_completion(working_memory, 0.5);
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
        Conversation working_memory = Conversation();
        working_memory.SetSystemData("Enrich json object to readable answer.\n"
            "Do not output any additional reasoning other than the enriched result.\n"
            "Below is json content to enrich:\n\n" + 
            json_content
        );
        Response response = chat_completion(working_memory, 0.5);
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
    }*/

};


