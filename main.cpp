#include <iostream>
#include <boost/beast.hpp>
#include <string>
#include <crypto++/sha.h>
#include <crypto++/hex.h>

#include "HttpServer.h"
#include "sql_create.h"

std::string hashPassword(const std::string& password) {
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

int main() {
    //sql_create();

    std::string s{"hello world"};
    std::cout << hashPassword(s) << std::endl;

    // const std::string db_path = "users.db";
    // HttpServer server(db_path);
    // server.run();
}
