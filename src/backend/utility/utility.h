#pragma once


namespace sharepaste
{
    std::string generateRandomString(size_t length);
    std::string databasePath(std::string_view subfolder, std::string_view filename);
}