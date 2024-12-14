// sql_create.cpp
#include "sql_create.h"
#include <sqlite3.h>
#include <iostream>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

std::string hashPassword(const std::string &password) {
    CryptoPP::SHA256 hash;
    std::string digest;

    CryptoPP::StringSource ss(password, true,new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));
    return digest;
}

int sql_create() {
    sqlite3* db;
    char* errMsg = 0;

    // Open a database connection
    int rc = sqlite3_open("users.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    // SQL statement to create a table
    const char* sql_create_table =
    "CREATE TABLE IF NOT EXISTS users ("
    "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "   name TEXT NOT NULL,"
    "   email TEXT NOT NULL UNIQUE,"
    "   role TEXT NOT NULL,"
    "   password TEXT NOT NULL"
    ");";

    rc = sqlite3_exec(db, sql_create_table, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    sqlite3_stmt *stmt=nullptr;
    const char* sql_insert_data = "INSERT INTO users (name, email, role, password) VALUES (?,?,?,?);";

    if (sqlite3_prepare_v2(db, sql_insert_data, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare SQL query: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, "admin", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, "admin@example.com", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "admin", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, hashPassword("admin").c_str(),-1,SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert data: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Data inserted successfully!" << std::endl;
    }

    // Finalize the statement to release resources
    sqlite3_finalize(stmt);

    // Close the connection
    sqlite3_close(db);

    return 0;
}