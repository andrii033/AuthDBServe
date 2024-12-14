//
// Created by korgot on 12/1/24.
//

#ifndef HTTPSERVER_H
#define HTTPSERVER_H


#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

using namespace boost::beast;
using namespace boost::asio;
using tcp = ip::tcp;

class HttpServer {
public:
    explicit HttpServer(const std::string &db_path)
        : db_path_(db_path), ioc_(1), acceptor_(ioc_, {tcp::v4(), 8080}) {
    }

    void run();

private:
    std::string db_path_;
    io_context ioc_;
    tcp::acceptor acceptor_;
    std::unordered_map<std::string, std::string> session_store;
    std::string hashPassword(const std::string& password);
    std::string read_file_to_string(const std::string &file_path);

    static std::string url_decode(const std::string &encoded);

    void handle_request(http::request<http::string_body> req, http::response<http::string_body> &res);
    static std::map<std::string, std::string> parse_form_data(const std::string &body);
    std::string generate_session_id();
    bool is_authenticated(const http::request<http::string_body> &req);
};


#endif //HTTPSERVER_H
