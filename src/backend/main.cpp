
#include <print>
#include <string>
#include <string_view>
#include <filesystem>
#include <random>
#include <algorithm>

#include <sqlite3.h>
#include <httplib.h>

#include <dbmanager.h>

managerSQL G_DATABASE;


std::string generateRandomString(size_t length)
{
    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());

    /*
    std::string randomString(characters);
    std::shuffle(randomString.begin(), randomString.end(), generator);
    
    return randomString.substr(0, length);
    */

    std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);

    std::string randomString;
    for (size_t i = 0; i < length; i++)
    {
        randomString += characters[distribution(generator)];
    }

    return randomString;
    
}


void postRequestNewPaste(const httplib::Request &req, httplib::Response &res)
{
    int codeLength {10};   
    std::string uniqueCode = std::format("{}", generateRandomString(codeLength));
    std::string shareLink = std::format("https://paste.charlestail.net/p/{}", uniqueCode);
    res.set_content(shareLink, "text/plain");

    std::println("[POST] Request New Paste Entry - {}", shareLink);

    std::string pasteData = req.get_param_value("pasteBody");

    // to-do Change front end request type from JSON to some sort of form
    std::println("params {}", req.params);
    std::println("body {}", req.body);
    req.body
    std::println("content length {}", req.content_length_);
    std::println("form {}", req.form.has_field("pasteBody"));
    std::println("headers {}", req.headers);


}  

void getServeFrontEnd(const httplib::Request &req, httplib::Response &res)
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
    svr.Post("/api", postRequestNewPaste);

    std::println("[Register] Adding get / handler");
    svr.Get(R"(/)", getServeFrontEnd);

    auto ret = svr.set_mount_point("/", "./www");
    if (!ret)
    {
        std::println("Cant mount /www to ./");
    }

    std::string host  = "0.0.0.0";
    int port = 80;

    std::println("[Info] Attempting to listen on {}:{}", host, port);

    if (!svr.listen(host, port)) {
        std::cerr << "[ERROR] Failed to bind to " << host << ":" << port
                  << ". Maybe another program is using it?\n";
        return -1;
    }
}