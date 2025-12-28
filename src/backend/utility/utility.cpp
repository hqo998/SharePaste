#include <random>
#include <string>
#include <algorithm>
#include <filesystem>
#include <print>
#include <iostream>

#include <cstdlib>

#include <utility.h>

#include <httplib.h>



namespace sharepaste
{
    std::string generateRandomString(size_t length)
    {
        const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());

        std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);

        std::string randomString;
        for (size_t i = 0; i < length; i++)
        {
            randomString += characters[distribution(generator)];
        }

        return randomString;
    }

    std::string databasePathConstructor(std::string_view subfolder, std::string_view filename)
    {
        printLine("[Test] Checking for database folder");

        std::string folderpathExists { std::format("./{}", subfolder)};
        if (!std::filesystem::exists(folderpathExists))
        {
            std::filesystem::create_directory(folderpathExists);

            printLine("[Pass] Creating database folder");
        }

        printLine("[Pass] Found database folder");
        return std::format("./{}/{}", subfolder, filename);
    }

    std::string getReqClientInfoString(const httplib::Request &req)
    {
        RequestInfo info {getReqClientInfoParse(req)};

        return std::format(
            "Request Data | URL-Path: '{}' / User-Agent: '{}' / Browser: '{}' / Platform: '{}' / Mobile: '{}' / IP: '{}:{}'",
            info.urlPath,
            info.userAgent,
            info.browser,
            info.platform,
            info.mobile,
            info.ip,
            info.port
        );
    }

    RequestInfo getReqClientInfoParse(const httplib::Request &req)
    {
        auto get = [&req](std::string h) -> std::string
        {
            return req.has_header(h) ? req.get_header_value(h) : "Empty";
        };

        RequestInfo info;

        info.userAgent = get("User-Agent");        // user agent
        info.browser = get("sec-ch-ua");            // browser
        info.platform = get("sec-ch-ua-platform");  // platform
        info.mobile = get("sec-ch-ua-mobile");      // is mobile?
        info.ip = req.remote_addr;                  // request ip address
        info.port = req.remote_port;                // request ip port
        info.urlPath = req.path;

        return info;
    }

    std::string fetchEnv(const std::string_view varEnv)
    {
        const char* val = std::getenv(std::string(varEnv).c_str());
        if (val == nullptr)
        {
            // add empty logic... check .env file?
            return std::string();
        }
        return std::string { val };
    }

    const std::string_view trimLeadingChar(std::string_view word, const std::string_view charToRemove)
    {
        if (!word.empty())
        {
            word.remove_prefix(std::min(word.find_first_not_of(charToRemove), word.size()));
            word.remove_suffix(std::min((word.size() - word.find_last_not_of(charToRemove) - 1), word.size()));
        }
        return word;
    }

    std::vector<std::string> stringToSplitArray(const std::string_view arrayString, const std::string_view splitChar)
    {
        std::vector<std::string> result;

        std::size_t lastFound { 0 };
        std::size_t found = arrayString.find_first_of(splitChar);

        while (found != std::string::npos)
        {
            auto token = trimLeadingChar(arrayString.substr(lastFound, found-lastFound));

            if (lastFound != found && !token.empty()) // accounts for commas doubled up and empty spaced commas
            {
                result.emplace_back(token);
            }

            lastFound = found + 1;
            found = arrayString.find_first_of(splitChar, found+1);
        }

        auto token = trimLeadingChar(arrayString.substr(lastFound));

        if (lastFound != found && !token.empty()) // accounts for commas doubled up
        {
            result.emplace_back(token);
        }

        return result;
    }
}

