#include <random>
#include <string>
#include <algorithm>
#include <filesystem>
#include <print>

#include <utility.h>

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

    std::string databasePath(std::string_view subfolder, std::string_view filename)
    {
        std::println("[Test] Checking for database folder");

        std::string folderpathExists { std::format("./{}", subfolder)};
        if (!std::filesystem::exists(folderpathExists))
        {
            std::filesystem::create_directory(folderpathExists);

            std::println("[Pass] Creating database folder");
        }

        std::println("[Pass] Found database folder");
        return std::format("./{}/{}", subfolder, filename);
    }
}

