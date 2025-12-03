#include <iostream>
#include <string>
#include <filesystem>
#include <print>

#include <sqlite3.h>
#include <httplib.h>


void getRequestAPI(const httplib::Request &, httplib::Response &res)
{
    res.set_content("Hello world!", "text/plain");
}

void serveFrontEnd(const httplib::Request &, httplib::Response &res)
{
    res.set_file_content("./www/index.html");
}

void checkMissingFrontend()
{
    std::println("[TEST] Looking for web directory");

    const std::string webroot = "./www";

    if (!std::filesystem::exists(webroot)) 
    std::println("[WARNING] Web directory not found");
    else
    std::println("[PASS] Found web directory");
}

void checkMissingFiles()
{
    checkMissingFrontend();
}

int main()
{
    std::println("[START] Beginning Sharenote");

    httplib::Server svr;

    checkMissingFiles();

    svr.Get("/api", getRequestAPI);

    svr.Get("/", serveFrontEnd);

    svr.listen("0.0.0.0", 80);
}