#pragma once

#include <string>

namespace sharepaste
{
    struct RequestInfo
    {
        std::string userAgent;
        std::string browser;
        std::string mobile;
        std::string platform;
        std::string urlPath;
        std::string ip;
        int port {};
    };

    std::string generateRandomString(size_t length);
    std::string databasePathConstructor(std::string_view subfolder, std::string_view filename);
    std::string getReqClientInfoString(const httplib::Request &req);
    RequestInfo getReqClientInfoParse(const httplib::Request &req);
}