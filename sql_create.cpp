// sql_create.cpp
#include "sql_create.h"
#include <sqlite3.h>
#include <iostream>

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

    // SQL statement to insert data
    const char* sql_insert_data =
    "INSERT INTO users (name, email, role, password) VALUES "
    "('Alice', 'alice@example.com', 'admin', '12345'),"
    "('Bob', 'bob@example.com', 'user', '12345'),"
    "('Charlie', 'charlie@example.com', 'guest', '12345');";

    rc = sqlite3_exec(db, sql_insert_data, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    // Close the connection
    sqlite3_close(db);

    std::cout << "Database created and populated with sample data successfully." << std::endl;
    return 0;
}