
#include <print>
#include <string>
#include <string_view>
#include <filesystem>
#include <random>
#include <algorithm>
#include <optional>

#include <sqlite3.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <dbmanager.h>

using json = nlohmann::json;

namespace sharepaste
{
    managerSQL G_DATABASE;
}


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


void postRequestAPINewPaste(const httplib::Request &req, httplib::Response &res) // set up some sort of rate limiting
{
    // Check for invalid post request
    if (!req.has_header("Content-Length") || req.body.empty())
    {
      auto val = req.get_header_value("Content-Length");

      std::println("[POST - API NEW] INVALID Request has issues");
      res.set_content("Request Invalid! Malformed", "text/plain");
      return;
    }

    // Check for crazy large payload
    if (req.body.size() > 100000)
    {
        std::println("[POST - API NEW] INVALID Request is too large");
        res.status = httplib::StatusCode::BadRequest_400;
        res.set_content("Request Invalid! Too Large...", "text/plain");
        return;
    }

    // Parse Post Request
    json bodyData = json::parse(req.body);
    std::optional<std::string> pasteBody = bodyData.at("pasteBody");

    // Invalid if string body is empty - should also add client side check
    if (pasteBody.value_or("").empty())
    {
        std::println("[POST - API NEW] INVALID Empty paste text body");
        res.status = httplib::StatusCode::BadRequest_400;
        res.set_content("Request Invalid! No Text...", "text/plain");
        return;
    }
    // std::println("Paste Text - {}", pasteBody.value()); // Print data from test body for debugging.


    // Generate random code
    int uniqueCodeLength {15};   // Roughly 3,527,930,788,646,880 possiblities, chance of a conflict is slim and if it does happen just have the user try the request again ez pz.
    std::string uniqueCode = std::format("{}", generateRandomString(uniqueCodeLength));

    bool insert_success = sharepaste::G_DATABASE.insertPaste(uniqueCode, pasteBody.value(), std::nullopt, std::nullopt);
    if (!insert_success)
    {
        std::println("[POST - API NEW] Insert Failed");
        res.status = httplib::StatusCode::InternalServerError_500;
        res.set_content("Request Invalid! Server Failed", "text/plain");
        return;
    }

    // if nothing returned early then respond with the sharelink
    res.set_content(uniqueCode, "text/plain");
    std::println("[POST - API NEW] New Paste Entry - {}", uniqueCode);

}


void getRequestPasteData(const httplib::Request &req, httplib::Response &res)
{
    std::println("[GET - Paste Data] Recieved");

    std::string uniqueCode {"NO CODE PROVIDED"};

    // check if code param exists
    if (req.has_param("code"))
    {
        uniqueCode = req.get_param_value("code");
    }
    else
    {
        res.status = httplib::StatusCode::BadRequest_400;
        res.set_content("Nothing.", "text/plain");
        return;
    }

    // Getting database info from code
    std::println("[GET - Paste Data] Fetching Data");
    std::optional<pasteData> retrievedPaste = sharepaste::G_DATABASE.getPasteData(uniqueCode);

    // Code has no data associated
    if (!retrievedPaste.has_value())
    {
        res.status = httplib::StatusCode::ImATeapot_418;
        res.set_content("Nothing.", "text/plain");
        return;
    }


    json responsePayload;
    responsePayload["pasteBody"] = retrievedPaste->pasteText;
    responsePayload["viewCount"] = retrievedPaste->viewCount;

    // updating view count stat
    sharepaste::G_DATABASE.updateViewCount(uniqueCode, retrievedPaste->viewCount + 1);

    // send out that json babbbbyyyyy
    std::println("[GET - Paste Data] JSON - {}", responsePayload.dump());
    res.set_content(responsePayload.dump(), "text/json");
}


void getPasteWebpage(const httplib::Request &req, httplib::Response &res)
{
    std::println("[GET - Webpage] Recieved");

    // this might be useless since the code here does nothing
    // since it was a holdover from the first idea of implementing the idea.
    std::string urlPath = req.path;
    std::string uniqueCode = urlPath.erase(0, 1);
    if (uniqueCode.empty())
    {
        std::println("URL Path - {} | uniqueCode - {}", urlPath, uniqueCode);
    }

    // serves script.js and style.css that are statically mounted at /www.
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


void runTests()
{
    // to do - add more tests and stuff
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
            runTests();
            exit(0);
        }
    }

    std::println("[START] Beginning SharePaste");

    httplib::Server svr;

    const std::string database_subfolder = "data";
    const std::string database_filename = "sharepaste.db";

    sharepaste::G_DATABASE.connect(databasePath(database_subfolder, database_filename));

    std::println("[Create Table] Creating table");
    sharepaste::G_DATABASE.createPasteTable();

    std::println("[Register] Adding get /api/new handler");
    svr.Post("/api/new", postRequestAPINewPaste);

    std::println("[Register] Adding get /api/find handler");
    svr.Get("/api/find", getRequestPasteData);

    std::println("[Register] Adding get /* handler");
    svr.Get(R"(.*)", getPasteWebpage);

    auto ret = svr.set_mount_point("/www", "./www");
    if (!ret)
    {
        std::println("Cant mount /www to ./www");
    }

    std::string host  = "0.0.0.0";
    int port = 8080;

    std::println("[Info] Attempting to listen on {}:{}", host, port);

    if (!svr.listen(host, port)) {
        std::cerr << "[ERROR] Failed to bind to " << host << ":" << port
                  << ". Maybe another program is using it?\n";
        return -1;
    }
}