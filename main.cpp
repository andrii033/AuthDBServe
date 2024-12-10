#include <iostream>
#include <boost/beast.hpp>
#include <string>


#include "HttpServer.h"
#include "sql_create.h"

int main() {
    //sql_create();

    const std::string db_path = "users.db";
    HttpServer server(db_path);
    server.run();
}
