//
// Created by korgot on 12/6/24.
//

#include "Data.h"

#include <iostream>

Data::Data(): db_path_("users.db") {
    rc = sqlite3_open("users.db", &db);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    }
}

Data::~Data() {
    if (db) {
        sqlite3_close(db);
    }
}

std::string Data::get_user_data_by_name(const std::string &name) {
    sqlite3_stmt *stmt;
    std::string result;

    if (sqlite3_prepare_v2(db, "SELECT password FROM users WHERE name = ?", -1, &stmt, nullptr)
            != SQLITE_OK) {
        sqlite3_close(db);
        return "Query preparation failed";
    }

    // Bind the parameter
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    } else {
        result = "User not found";
    }

    sqlite3_finalize(stmt); // Finalize to clean up after query execution

    return result;
}
