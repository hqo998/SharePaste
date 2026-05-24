#include <print>
#include <filesystem>
#include <utility.h>

#include <testtools.h>
#include <ratelimiter.h>

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

        const std::string index = "./index.html";
        if (!std::filesystem::exists(index))
        {
            printLine("[WARNING] Index.html not found");
            std::exit(-1);
        }
        else
            printLine("[PASS] Index.html web directory");
    }

    void rateTest()
    {
        IpRateLimiter ipManager;
        ipManager.allowRequest(std::string("192.168.0.1111"), 1);
        ipManager.allowRequest(std::string("192.168.0.420"), 1);
        std::this_thread::sleep_for(std::chrono::seconds(90));
        ipManager.allowRequest(std::string("192.168.0.1111"), 1);
        ipManager.cleanAll(10);
        ipManager.allowRequest(std::string("192.168.0.1111"), 1);
        ipManager.printAllIps();
        exit(-1);
    }
}
