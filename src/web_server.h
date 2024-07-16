#pragma once
#pragma push_macro("guard")
#pragma push_macro("unguard")
#undef guard
#undef unguard

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/filesystem.hpp>

#pragma pop_macro("guard")
#pragma pop_macro("unguard")

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

#define ROOT_PAGE "web_ui/chat.html"
#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_HTTP_PORT 8080
#define DEFAULT_WS_PORT 9002

class WebServer {
private:
    std::string __address;
    unsigned short __ws_port;
    unsigned short __http_port;
    std::set<websocket::stream<tcp::socket>*> connections;
    std::mutex connection_lock;

    void handle_websocket_session(tcp::socket socket) {
        websocket::stream<tcp::socket> ws{std::move(socket)};
        try {
            fmt::print("New WebSocket connection established\n");
            ws.accept();
            {
                std::lock_guard<std::mutex> lock(connection_lock);
                connections.insert(&ws);
            }
            beast::flat_buffer buffer;
            while (true) {
                ws.read(buffer);
                auto msg = beast::buffers_to_string(buffer.data());
                buffer.consume(buffer.size());
                fmt::print("WebSocket message received: {}\n", msg);
                {
                    std::lock_guard<std::mutex> lock(connection_lock);
                    for (auto* connection : connections) {
                        connection->text(ws.got_text());
                        connection->write(net::buffer(msg));
                    }
                }
            }
        } catch (beast::system_error const& se) {
            if (se.code() != websocket::error::closed) {
                fmt::print("WebSocket Error: {}\n", se.code().message());
            }
        } catch (std::exception const& e) {
            fmt::print("WebSocket Exception: {}\n", e.what());
        }
        {
            std::lock_guard<std::mutex> lock(connection_lock);
            connections.erase(&ws);
        }
        fmt::print("WebSocket connection closed\n");
    }

    void handle_http_request(tcp::socket socket) {
        bool close = false;
        beast::flat_buffer buffer;
        try {
            http::request<http::string_body> req;
            http::read(socket, buffer, req);
            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.keep_alive(req.keep_alive());
            fmt::print("HTTP request received: {}\n", std::string(req.target()));
            if (req.target() == "/") {
                handle_root_target(req, res);
            } else if (req.target() == "/upload" && req.method() == http::verb::post) {
                handle_upload_target(req, res);
            } else {
                handle_not_found_target(res);
            }
            res.prepare_payload();
            http::write(socket, res);
            close = res.need_eof();
        } catch (std::exception const& e) {
            fmt::print("HTTP Error: {}\n", e.what());
            close = true;
        }
        if (close) {
            beast::error_code ec;
            socket.shutdown(tcp::socket::shutdown_send, ec);
            if (ec) {
                fmt::print("Shutdown error: {}\n", ec.message());
            }
        }
    }

    void handle_root_target(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
        (void)req;
        std::ifstream ifs(ROOT_PAGE);
        if (ifs) {
            std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
            res.set(http::field::content_type, "text/html");
            res.body() = content;
        } else {
            res.result(http::status::not_found);
            res.body() = "The resource was not found.";
        }
    }

    void handle_upload_target(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
        auto body = json::parse(req.body());
        for (auto& element : body["files"]) {
            std::string filename = element["filename"];
            std::string content = element["content"];
            std::ofstream outfile(filename, std::ios::binary);
            outfile << content;
            outfile.close();
        }
        res.body() = "Files uploaded successfully!";
    }

    void handle_not_found_target(http::response<http::string_body>& res) {
        res.result(http::status::not_found);
        res.body() = "The resource was not found.";
    }

    void start_websocket_server() {
        guard("WebServer::start_websocket_server")
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc};
        if (!setup_acceptor(acceptor, __address, __ws_port, "WebSocket")) { return; }
        while (true) {
            tcp::socket socket{ioc};
            boost::system::error_code ec;
            acceptor.accept(socket, ec);
            if (!ec) {
                std::thread(&WebServer::handle_websocket_session, this, std::move(socket)).detach();
            } else {
                fmt::print("Error accepting connection: {}\n", ec.message());
            }
        }
        unguard()
    }

    void start_http_server() {
        guard("WebServer::start_http_server")
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc};
        if (!setup_acceptor(acceptor, __address, __http_port, "HTTP")) { return; }
        while (true) {
            tcp::socket socket{ioc};
            boost::system::error_code ec;
            acceptor.accept(socket, ec);
            if (!ec) {
                std::thread(&WebServer::handle_http_request, this, std::move(socket)).detach();
            } else {
                fmt::print("Error accepting connection: {}\n", ec.message());
            }
        }
        unguard()
    }

    bool setup_acceptor(tcp::acceptor& acceptor, 
        const std::string& address_str, 
        unsigned short port, 
        std::string server_type
    ) {
        try {
            auto address = net::ip::make_address(address_str);
            acceptor.open(address.is_v4() ? tcp::v4() : tcp::v6());
            acceptor.bind({address, port});
            acceptor.listen(net::socket_base::max_listen_connections);
            fmt::print("{} server running on {}://{}:{}\n", server_type, server_type == "WebSocket" ? "ws" : "http", address_str, port);
            return true;
        } catch (const boost::system::system_error& e) {
            fmt::print("Error setting up acceptor: {}\n", e.what());
            return false;
        }
    }

public:
    WebServer(const std::string& address = DEFAULT_ADDRESS, 
        unsigned short ws_port = DEFAULT_WS_PORT, 
        unsigned short http_port = DEFAULT_HTTP_PORT)
        : __address(address), __ws_port(ws_port), __http_port(http_port) {}

    void start() {
        guard("WebServer::start")
        std::thread websocket_thread(&WebServer::start_websocket_server, this);
        std::thread http_thread(&WebServer::start_http_server, this);
        websocket_thread.join();
        http_thread.join();
        unguard()
    }

    /// Accessors
    std::string get_address() const { return __address; }
    void set_address(const std::string& address) { __address = address; }
    unsigned short get_ws_port() const { return __ws_port; }
    void set_ws_port(unsigned short port) { __ws_port = port; }
    unsigned short get_http_port() const { return __http_port; }
    void set_http_port(unsigned short port) { __http_port = port; }

};
