#pragma once

#include "core.h"
#include "logger.h"
#include "liboai.h"

///
/// @brief LLM client
///
class LLM {

        liboai::OpenAI oai;
        std::string llm_model;

        Logger* logger;

    public:

        LLM() {
            logger = Logger::get_instance();
        }

        void set_model(std::string model) {
            llm_model = model;
        }

       void set_provider(std::string endpoint, std::string key) {
            bool result = oai.auth.SetKey(key);
            if (!result) {
                throw std::runtime_error("Failed to set API key");
            }
            oai.ChatCompletion->SetEndpoint(endpoint);
        }

        liboai::Response chat_completion(liboai::Conversation& conversation, float temperature) {
            guard("LLM::chat_completion")
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

        vdb::vector embedding(const std::string& text) {
            guard("LLM::embedding")
            liboai::Response response = oai.Embedding->create(
                "text-embedding-ada-002",
                //"text-embedding-3-large",
                text
            );
            json jres = response["data"][0]["embedding"];
            return vdb::vector({ jres.begin(), jres.end() });
            unguard()
            return vdb::vector();
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
