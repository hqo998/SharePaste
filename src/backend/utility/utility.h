#pragma once

#include <string>

namespace sharepaste
{
    typedef struct RequestInfo
    {
        std::string userAgent;
        std::string browser;
        std::string mobile;
        std::string platform;
        std::string urlPath;
        std::string ip;
        int port {};
    } RequestInfo;

    std::string generateRandomString(size_t length);
    std::string databasePathConstructor(std::string_view subfolder, std::string_view filename);
    std::string extractReqInfo(const httplib::Request &req);
}