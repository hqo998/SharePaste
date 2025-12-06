
#include <print>
#include <string>
#include <string_view>
#include <filesystem>

#include <sqlite3.h>
#include <httplib.h>

#include <dbmanager.h>

managerSQL G_DATABASE;

void getRequestAPI(const httplib::Request &, httplib::Response &res)
{
    std::println("[GET] Request API");
    res.set_content("Hello world!", "text/plain");
}

void getServeFrontEnd(const httplib::Request &, httplib::Response &res)
{
    std::println("[GET] Request FrontEnd");
    res.set_file_content("./www/index.html", "text/html");
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

void checkMissingFiles()
{
    checkMissingFrontend();
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
    

int main(int argc, char* argv[])
{
    if (argc > 1) // Check CLI arguements
    {
        if (strcmp(argv[1], "--test") == 0)
        {
            std::println("[START] Running --tests");
            checkMissingFiles();
            exit(0);
        }
    }
    

    std::println("[START] Beginning SharePaste");

    httplib::Server svr; 

    const std::string database_subfolder = "data";
    const std::string database_filename = "sharepaste.db";
 

    G_DATABASE.connect(databasePath(database_subfolder, database_filename));

    G_DATABASE.createPasteTable();

    std::println("[Register] Adding get /api handler");
    svr.Get("/api", getRequestAPI);

    std::println("[Register] Adding get / handler");
    svr.Get(R"(/)", getServeFrontEnd);

    std::string host  = "0.0.0.0";
    int port = 80;

    std::println("[Info] Attempting to listen on {}:{}", host, port);

    if (!svr.listen(host, port)) {
        std::cerr << "[ERROR] Failed to bind to " << host << ":" << port
                  << ". Maybe another program is using it?\n";
        return -1;
    }
}