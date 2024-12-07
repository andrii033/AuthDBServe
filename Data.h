//
// Created by korgot on 12/6/24.
//

#ifndef DATA_H
#define DATA_H
#include <sqlite3.h>
#include <string>


class Data {
public:
    Data();
    ~Data();
    std::string get_user_data_by_name(const std::string &name);

private:
    std::string db_path_;
    sqlite3* db;
    char* errMsg = 0;
    int rc;
};

#endif //DATA_H
