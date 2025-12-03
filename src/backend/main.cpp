#include <iostream>
#include <print>
#include <string>
#include <filesystem>

#include <sqlite3.h>
#include <httplib.h>


void getRequestAPI(const httplib::Request &, httplib::Response &res)
{
    std::println("[GET] Request API");
    res.set_content("Hello world!", "text/plain");
}

void serveFrontEnd(const httplib::Request &, httplib::Response &res)
{
    std::println("[GET] Request FrontEnd");
    res.set_file_content("./www/index.html", "text/html");
}


void checkMissingFrontend()
{
    std::println("[TEST] Looking for web frontend");

    const std::string webroot = "./www";
    if (!std::filesystem::exists(webroot))
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

void checkMissingFiles()
{
    checkMissingFrontend();
}

int main(int argc, char* argv[])
{
    // TO-DO get this cli arguement working for testing during docker build
    if (argc > 0)
    {
        if (argv[1] == "--test")
        {
            std::println("[START] Running Tests Only");
            checkMissingFiles();
            exit(0);
        }
    }
    std::println("Argc {}, argv {}", argc, argv[1]);
    
    std::println("[START] Beginning SharePaste");

    httplib::Server svr;

    checkMissingFiles();

    std::println("[Register] Adding get /api");
    svr.Get("/api", getRequestAPI);

    std::println("[Register] Adding get /");
    svr.Get(R"(/.*)", serveFrontEnd);

    std::string host  = "0.0.0.0";
    int port = 80;

    std::println("[Info] Attempting to listen on {}:{}", host, port);

    if (!svr.listen(host, port)) {
        std::cerr << "[ERROR] Failed to bind to " << host << ":" << port
                  << ". Maybe another program is using it?\n";
        return -1;
    }
    
}