#pragma once

#include <string>
#include <string_view>
#include <sqlite3.h>


struct PasteData
{
    std::string uniqueCode {};
    std::string pasteText {};
    std::string createdDate {};
    std::optional<std::string> expiryDate {};
    std::optional<std::string> codeType {};
    bool burnAfterRead {};
    int viewCount {};
    int reports {};
};


class managerSQL
{
public:
    sqlite3* db = nullptr;

    void connect(const std::string& filename);
    void createPasteTable();
    void execute(const std::string &command);
    void closeDB();
    void createTable(const std::string_view& tableName, const std::string_view& columns);
    bool updateViewCount(const std::string &uniqueCode, int newViewCount);
    std::optional<PasteData> getPasteData(const std::string &uniqueCode);
    bool insertPaste(const std::string &uniqueCode, const std::string &pasteText, std::optional<std::string> expiresAt, std::optional<std::string> syntax);

    bool addColumnIfNotExists(const std::string& newColumn, const std::string& columnDef);

    void insertData(const std::string_view &tableName, const std::string_view &columns, const std::string_view &fields);
    void deleteData(const std::string_view& tableName, int id);
};
