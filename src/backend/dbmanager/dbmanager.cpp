#include <iostream>
#include <print>
#include <string>
#include <format>
#include <sqlite3.h>


#include "dbmanager.h"


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
            syntax TEXT,
            title TEXT,
            burn_after_read BOOLEAN DEFAULT 0,
            view_count INTEGER DEFAULT 0
            );
        )";
    execute(createTableCommand);
}

void managerSQL::execute(const std::string& Command)
{
    char* errMsg;
    int rc = sqlite3_exec(db, Command.c_str(), NULL, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "[DB Execute] Error in executing command: " <<  errMsg << "\n";
        sqlite3_free(errMsg);
    }
    else
    {
        std::println("[DB Execute] Executed.");

    }
}

void managerSQL::createTable(const std::string_view& tableName, const std::string_view& columns)
{
    char* errMsg;
    std::string sql = std::format("CREATE TABLE IF NOT EXISTS {0}({1});", tableName, columns);
    int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error in executing CREATE TABLE SQL: " <<  errMsg << "\n";
        sqlite3_free(errMsg);
    }
    else
    {
        std::cout << "table " << tableName << " made successfully" << '\n';

    }
}

void managerSQL::insertData(const std::string_view& tableName, const std::string_view& columns, const std::string_view& fields)
{
    char* errMsg;
    std::string sql = std::format("INSERT OR IGNORE INTO {0}({1}) VALUES{2};", tableName, columns, fields);
    int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error in executing INSERT SQL: " << errMsg << "\n";
        //free the error message
        sqlite3_free(errMsg);
    }
    else
    {
        std::cout << "Insert operation successful" << '\n';

    }
}

void managerSQL::deleteData(const std::string_view& tableName, int id)
{
    char* errMsg;
    std::string sql = std::format("DELETE FROM '{0}' WHERE ID={1};", tableName, std::to_string(id));

    int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error in executing DELETE SQL: " << errMsg << "\n";
        //free the error message
        sqlite3_free(errMsg);
    }
    else 
    {
        std::cout << "Delete operation successful" << '\n';

    }
}

void managerSQL::closeDB()
{
    if (db) sqlite3_close(db);
}



void example()
{
    managerSQL database;

    std::string filename = "./example.db";
    database.connect(filename);

    std::string tableName = "STUDENTS";
    std::string columns = "ID INTEGER PRIMARY KEY AUTOINCREMENT, FIRST_NAME TEXT NOT NULL, LAST_NAME TEXT NOT NULL, EMAIL TEXT UNIQUE NOT NULL";
    database.createTable(tableName, columns);

    std::string dataFormat = "FIRST_NAME, LAST_NAME, EMAIL";
    std::string data = "('Joe', 'Smith', 'joe@smith.com'), ('Holly', 'Bracey', 'holly@bracey.com')";
    database.insertData(tableName, dataFormat, data);

    int id = 2;
    database.deleteData(tableName, id);

    database.closeDB();
}
