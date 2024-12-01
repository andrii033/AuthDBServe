//
// Created by korgot on 12/1/24.
//

#include "HttpServer.h"


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

std::string HttpServer::get_user_data_from_db(const std::string &role) {
    sqlite3 *db;
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        return "Failed to open database";
    }

    std::string query;
    if (role == "admin") {
        query = "SELECT * FROM users;";
    } else if (role == "user") {
        query = "SELECT id, name FROM users;";
    } else {
        sqlite3_close(db);
        return "Access Denied";
    }

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return "Query preparation failed";
    }

    std::string result = "[";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result += "{";
        result += "\"id\": " + std::to_string(sqlite3_column_int(stmt, 0)) + ", ";
        result += "\"name\": \"" + std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1))) + "\"";
        if (role == "admin") {
            result += ", \"email\": \"" + std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))) +
                    "\"";
        }
        result += "}, ";
    }
    if (result.size() > 1) {
        result.pop_back(); // Remove the extra comma
        result.pop_back();
    }
    result += "]";

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
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

void HttpServer::handle_request(http::request<http::string_body> req, http::response<http::string_body> &res) {
        std::string target = req.target();

        std::cout << "Target: " << target << std::endl;

        if (target == "/") {
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
        } else if (target == "/login" && req.method() == http::verb::get) {
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
        } else if (target == "/login" && req.method() == http::verb::post) {
            std::cout << "POST /login" << std::endl;
            std::string body = req.body();
            auto pos_username = body.find("username=");
            auto pos_password = body.find("password=");

            if (pos_username != std::string::npos && pos_password != std::string::npos) {
                std::string username = body.substr(pos_username + 9, pos_password - (pos_username + 9) - 1);
                std::string password = body.substr(pos_password + 9);

                if (username == "correct_username" && password == "correct_password") {
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
        } else {
            res.result(http::status::not_found);
            res.body() = "Not Found";
        }
        res.prepare_payload();
    }


