#pragma once

#include "core.h"
#include "logger.h"
#include "liboai.h"

using namespace liboai;

class EmbeddingsInterface {
protected:
    embedding_model embed_model;

public:
    virtual ~EmbeddingsInterface() = default;

    virtual void set_model(embedding_model model) {
        embed_model = model;
    }

    virtual embedding_model get_model() const {
        return embed_model;
    }

    virtual void set_provider(std::string endpoint, std::string api_key) = 0;

    virtual Response embeddings(
        const std::string& text,
        std::optional<std::string> user = std::nullopt
    ) const& noexcept(false) = 0;

    virtual std::future<Response> embeddings_async(
        const std::string& text,
        std::optional<std::string> user = std::nullopt
    ) = 0;
};


class EmbeddingsProvider final : public EmbeddingsInterface, private Network {
    Authorization& auth = Authorization::Authorizer();
    Logger* logger = Logger::get_instance();

public:

    EmbeddingsProvider() = default;
    ~EmbeddingsProvider() = default;
    EmbeddingsProvider(const EmbeddingsProvider&) = delete;
    EmbeddingsProvider(EmbeddingsProvider&&) = delete;

    EmbeddingsProvider& operator=(const EmbeddingsProvider&) = delete;
    EmbeddingsProvider& operator=(EmbeddingsProvider&&) = delete;

    void set_provider(std::string endpoint, std::string api_key) {
        bool result = auth.SetKey(api_key);
        if (!result) {
            throw std::runtime_error("Failed to set API key");
        }
        this->endpoint_root_ = endpoint;
    }

    Response embeddings(
        const std::string& text,
        std::optional<std::string> user = std::nullopt
    ) const& noexcept(false) {
	    guard("EmbeddingsProvider::embeddings")
        JsonConstructor jcon;
        jcon.push_back("model", fmt::format("{}", embed_model));
        jcon.push_back("input", std::move(text));
        jcon.push_back("user", std::move(user));
        Response response;
        response = this->Request(
            Method::HTTP_POST, this->endpoint_root_, "/embeddings", "application/json",
            this->auth.GetAuthorizationHeaders(),
            netimpl::components::Body {
                jcon.dump()
            },
            this->auth.GetProxies(),
            this->auth.GetProxyAuth(),
            this->auth.GetMaxTimeout()
        );
        return response; //vdb::vector({ jres.begin(), jres.end() }, model);
        unguard()
        return Response();
    }

    std::future<Response> embeddings_async(
        const std::string& text,
        std::optional<std::string> user = std::nullopt
    ) {
        return std::async(std::launch::async, &EmbeddingsProvider::embeddings, this, text, user);
    }

};
