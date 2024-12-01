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

    std::string get_user_data_from_db(const std::string &role);
    std::string read_file_to_string(const std::string &file_path);
    void handle_request(http::request<http::string_body> req, http::response<http::string_body> &res);
};


#endif //HTTPSERVER_H
