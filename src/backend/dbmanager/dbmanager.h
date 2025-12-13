#pragma once

#include <string>
#include <string_view>
#include <sqlite3.h>

class managerSQL
{
public:
    sqlite3* db = nullptr;

    void connect(const std::string& filename);
    void createPasteTable();
    void execute(const std::string &command);
    void closeDB();
    void createTable(const std::string_view& tableName, const std::string_view& columns);
    void insertData(const std::string_view &tableName, const std::string_view &columns, const std::string_view &fields);
    void deleteData(const std::string_view& tableName, int id);
    
};
