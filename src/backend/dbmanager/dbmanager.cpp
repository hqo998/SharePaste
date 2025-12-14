#include <iostream>
#include <print>
#include <string>
#include <format>
#include <sqlite3.h>


#include "dbmanager.h"

// to do - make a insert data that can prepare statements

void managerSQL::connect(const std::string& filename)
{

    int opened = sqlite3_open(filename.c_str(), &db);
    if (opened)
    {
        std::cerr << "[DB Connect] Failed to open DB: " << sqlite3_errmsg(db) << "\n";
    }
    else
    {
        std::println("[DB Connect] Opened Succesfully");
    }
}

void managerSQL::createPasteTable()
{
    std::string createTableCommand =
        R"(
            CREATE TABLE IF NOT EXISTS Pastes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            unique_code TEXT UNIQUE NOT NULL,
            paste_text TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            expires_at DATETIME,
            code_type TEXT,
            burn_after_read BOOLEAN DEFAULT 0,
            view_count INTEGER DEFAULT 0,
            reports INTEGER DEFAULT 0
            );
        )";
    execute(createTableCommand);
}

void managerSQL::execute(const std::string& command)
{
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, command.c_str(), NULL, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "[DB Execute] Error in executing command: " <<  errMsg << "\n";
        std::cerr << command << "\n";
        sqlite3_free(errMsg);
    }
    else
    {
        std::println("[DB Execute] Executed.");

    }
}

void managerSQL::createTable(const std::string_view& tableName, const std::string_view& columns)
{
    std::string sql = std::format("CREATE TABLE IF NOT EXISTS {0}({1});", tableName, columns);
    execute(sql);
}

std::optional<pasteData> managerSQL::getPasteData(const std::string& uniqueCode)
{
    std::optional<pasteData> retrievedData {std::nullopt};

    std::string sqlSelectCommand = "SELECT unique_code, paste_text, created_at, expires_at, code_type, view_count, reports "  
    "FROM Pastes WHERE unique_code = ? LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;

    int prep_insert = sqlite3_prepare_v2(db, sqlSelectCommand.c_str(), -1, &stmt, nullptr);
    if (prep_insert != SQLITE_OK)
    {
        std::println("[Get Paste] Prep Failed... {}", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return retrievedData;
    }

    sqlite3_bind_text(stmt, 1, uniqueCode.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        retrievedData.emplace();

        retrievedData.value().uniqueCode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        retrievedData.value().pasteText = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        retrievedData.value().createdDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        const unsigned char* expiry = sqlite3_column_text(stmt, 3);
        if (expiry)
            retrievedData.value().expiryDate = reinterpret_cast<const char*>(expiry);
        
        const unsigned char* codetype = sqlite3_column_text(stmt, 4);
        if (expiry)
            retrievedData.value().codeType = reinterpret_cast<const char*>(codetype);    

        retrievedData.value().viewCount = sqlite3_column_int(stmt, 5);
        retrievedData.value().reports = sqlite3_column_int(stmt, 6);
    }


    sqlite3_finalize(stmt);
    return retrievedData;
}

bool managerSQL::insertPaste(
    const std::string& uniqueCode,
    const std::string& pasteText,
    std::optional<std::string> expiresAt = std::nullopt,
    std::optional<std::string> code_type = std::nullopt
    // add method to insert items into db with prepared statements, std::optional??
    )
{
    sqlite3_stmt* stmt = nullptr;
    
    std::string sqlInsertCommand = "INSERT INTO Pastes ("
        "unique_code, paste_text, expires_at, code_type"
        ") VALUES (?, ?, ?, ?);";

    // preparing statements
    int prep_insert = sqlite3_prepare_v2(db, sqlInsertCommand.c_str(), -1, &stmt, nullptr);
    if (prep_insert != SQLITE_OK)
    {
        std::println("[Insert Paste] Prep Failed... {}", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    // binding parameters
    sqlite3_bind_text(stmt, 1, uniqueCode.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pasteText.c_str(), -1, SQLITE_TRANSIENT);
    if (expiresAt.has_value())
        sqlite3_bind_text(stmt, 3, expiresAt.value().c_str(), -1, SQLITE_TRANSIENT);
    else
        sqlite3_bind_null(stmt, 3);

    if (code_type.has_value())
        sqlite3_bind_text(stmt, 4, code_type.value().c_str(), -1, SQLITE_TRANSIENT);
    else
        sqlite3_bind_null(stmt, 4);


    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::println("[Insert Paste] Step failed... {}", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
        

    sqlite3_finalize(stmt);
    return true;

}


void managerSQL::insertData(const std::string_view& tableName, const std::string_view& columns, const std::string_view& fields)
{
    std::string sql = std::format("INSERT OR IGNORE INTO {0}({1}) VALUES{2};", tableName, columns, fields);
    execute(sql);
}

void managerSQL::deleteData(const std::string_view& tableName, int id)
{
    std::string sql = std::format("DELETE FROM '{0}' WHERE ID={1};", tableName, std::to_string(id));
    execute(sql);
}

void managerSQL::closeDB()
{
    if (db) sqlite3_close(db);
}