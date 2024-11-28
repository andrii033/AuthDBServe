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

std::string get_user_data_from_db(const std::string& role, const std::string& db_path) {
    sqlite3* db;
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
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

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return "Query preparation failed";
    }

    std::string result = "[";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result += "{";
        result += "\"id\": " + std::to_string(sqlite3_column_int(stmt, 0)) + ", ";
        result += "\"name\": \"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) + "\"";
        if (role == "admin") {
            result += ", \"email\": \"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))) + "\"";
        }
        result += "}, ";
    }
    if(result.size() > 1)
    {
        result.pop_back(); // Remove the extra comma
        result.pop_back();
    }
    result += "]";

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}

std::string read_file_to_string(const std::string& file_path) {
    std::ifstream file_stream(file_path);
    if (!file_stream) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

void handle_request(http::request<http::string_body> req, http::response<http::string_body>& res, const std::string& db_path) {
    std::string target = req.target();

    // Existing CORS and route handling logic...

    // if (target == "/") {
    //     // Serve an HTML page
    //     res.result(http::status::ok);
    //     res.set(http::field::content_type, "text/html");
    //     res.body() = "<html><body><h1>Welcome</h1><p>Use the API!</p></body></html>";
    // }

    if (target == "/") {
        try {
            std::string html_content = read_file_to_string("client.html");
            res.result(http::status::ok);
            res.set(http::field::content_type, "text/html");
            res.body() = html_content;
        } catch (const std::exception& e) {
            res.result(http::status::internal_server_error);
            res.body() = "Error loading HTML file";
            std::cerr << e.what() << std::endl;
        }
    }
}


int main() {
    try {
        const std::string db_path = "users.db";

        io_context ioc{1};
        tcp::acceptor acceptor{ioc, {tcp::v4(), 8080}};

        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            http::request<http::string_body> req;
            flat_buffer buffer;
            http::read(socket, buffer, req);

            http::response<http::string_body> res;
            handle_request(req, res, db_path);
            http::write(socket, res);
        }
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}