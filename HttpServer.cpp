//
// Created by korgot on 12/1/24.
//

#include "HttpServer.h"
#include "Data.h"


void HttpServer::run() {
    try {
        for (;;) {
            tcp::socket socket{ioc_};
            acceptor_.accept(socket);

            http::request<http::string_body> req;
            flat_buffer buffer;
            http::read(socket, buffer, req);

            http::response<http::string_body> res;
            handle_request(req, res);
            http::write(socket, res);
        }
    } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

std::string HttpServer::hashPassword(const std::string &password) {
    CryptoPP::SHA256 hash;
    std::string digest;

    CryptoPP::StringSource ss(password, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(digest)
            )
        )
    );

    return digest;
}

std::string HttpServer::read_file_to_string(const std::string &file_path) {
    std::ifstream file_stream(file_path);
    if (!file_stream) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

std::map<std::string, std::string> HttpServer::parse_form_data(const std::string &body) {
    std::map<std::string, std::string> parsed_data;
    std::stringstream ss(body);
    std::string key_value_pair;

    while (std::getline(ss, key_value_pair, '&')) {
        size_t pos = key_value_pair.find('=');
        if (pos != std::string::npos) {
            std::string key = key_value_pair.substr(0, pos);
            std::string value = key_value_pair.substr(pos + 1);
            parsed_data[key] = value;
        }
    }

    return parsed_data;
}

void HttpServer::handle_request(http::request<http::string_body> req, http::response<http::string_body> &res) {
    std::string target = req.target();

    std::cout << "Target: " << target << std::endl;

    if (target == "/") {
        // Data data;
        // //data.get_user_data_by_name("Alice");
        // std::cout << "Alice "<< data.get_user_data_by_name("Alice") << std::endl;

        try {
            std::string html_content = read_file_to_string("resources/client.html");
            res.result(http::status::ok);
            res.set(http::field::content_type, "text/html");
            res.body() = html_content;
        } catch (const std::exception &e) {
            res.result(http::status::internal_server_error);
            res.body() = "Error loading HTML file";
            std::cerr << e.what() << std::endl;
        }
    }
    if (target == "/login" && req.method() == http::verb::get) {
        std::cout << "GET /login" << std::endl;
        try {
            std::string login_page = read_file_to_string("resources/login.html");
            res.result(http::status::ok);
            res.set(http::field::content_type, "text/html");
            res.body() = login_page;
        } catch (const std::exception &e) {
            res.result(http::status::internal_server_error);
            res.body() = "Error loading login page";
            std::cerr << e.what() << std::endl;
        }
    }
    if (target == "/login" && req.method() == http::verb::post) {
        std::cout << "POST /login" << std::endl;
        std::string body = req.body();
        auto pos_username = body.find("username=");
        auto pos_password = body.find("password=");

        if (pos_username != std::string::npos && pos_password != std::string::npos) {
            std::string username = body.substr(pos_username + 9, pos_password - (pos_username + 9) - 1);
            std::string password = body.substr(pos_password + 9);

            if (username == "username" && password == "password") {
                res.result(http::status::ok);
                res.body() = "Login Successful!";
            } else {
                res.result(http::status::unauthorized);
                res.body() = "Invalid username or password";
            }
        } else {
            res.result(http::status::bad_request);
            res.body() = "Invalid form";
        }
    }
    if (target == "/register" && req.method() == http::verb::get) {
        std::cout << "GET /register" << std::endl;
        try {
            std::string register_page = read_file_to_string("resources/register.html");
            res.result(http::status::ok);
            res.set(http::field::content_type, "text/html");
            res.body() = register_page;
        } catch (const std::exception &e) {
            res.result(http::status::internal_server_error);
            res.body() = "Error loading register page";
        }
    }
    if (target == "/register" && req.method() == http::verb::post) {
        std::cout << "POST /register" << std::endl;
        std::cout << req.body() << std::endl;
        std::map<std::string,std::string> form_data = parse_form_data(req.body());
        std::cout << "Username: " << form_data["name"] << std::endl;
        std::cout << "Password: " << form_data["password"] << std::endl;
    }
    res.prepare_payload();
}

