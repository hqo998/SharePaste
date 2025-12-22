#include <print>
#include <filesystem>

#include <testtools.h>

namespace sharepaste
{
    void runTests()
    {
        // to do - add more tests and stuff
        std::println("[TEST] Starting backend test sequence...");
        checkMissingFrontend();
    }

    void checkMissingFrontend()
    {
        std::println("[TEST] Looking for web frontend");

        const std::string webRoot = "./www";
        if (!std::filesystem::exists(webRoot))
        {
            std::println("[WARNING] Web directory not found");
            std::exit(-1);
        }

        else
        std::println("[PASS] Found web directory");

        const std::string index = "./www/index.html";
        if (!std::filesystem::exists(index))
        {
            std::println("[WARNING] Index.html not found");
            std::exit(-1);
        }
        else
        std::println("[PASS] Index.html web directory");

    }
}
