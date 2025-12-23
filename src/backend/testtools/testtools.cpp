#include <print>
#include <filesystem>
#include <utility.h>

#include <testtools.h>

namespace sharepaste
{
    void runTests()
    {
        // to do - add more tests and stuff
        printLine("[TEST] Starting backend test sequence...");
        checkMissingFrontend();
    }

    void checkMissingFrontend()
    {
        printLine("[TEST] Looking for web frontend");

        const std::string webRoot = "./www";
        if (!std::filesystem::exists(webRoot))
        {
            printLine("[WARNING] Web directory not found");
            std::exit(-1);
        }

        else
        printLine("[PASS] Found web directory");

        const std::string index = "./www/index.html";
        if (!std::filesystem::exists(index))
        {
            printLine("[WARNING] Index.html not found");
            std::exit(-1);
        }
        else
        printLine("[PASS] Index.html web directory");

    }
}
