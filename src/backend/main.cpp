#include <print>
#include <string>
#include <string_view>
#include <optional>

#include <sqlite3.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <dbmanager.h>
#include <utility.h>
#include <testtools.h>

using json = nlohmann::json;


namespace sharepaste
{
    managerSQL G_DATABASE;
    inline constexpr int uniqueCodeLength {15};   // Roughly 3,527,930,788,646,880 possiblities, chance of a conflict is slim and if it does happen just have the user try the request again ez pz.
}

void postRequestAPINewPaste(const httplib::Request &req, httplib::Response &res) // set up some sort of rate limiting
{
    std::println("[POST - API NEW] Recieved.");
    std::println("{}", sharepaste::getReqClientInfoString(req));
    
    // Check for invalid post request
    if (!req.has_header("Content-Length") || req.body.empty())
    {
      auto val = req.get_header_value("Content-Length");

      std::println("[POST - API NEW] INVALID Request has issues.");
      res.set_content("Request Invalid! Malformed", "text/plain");
      return;
    }

    // Check for crazy large payload
    if (req.body.size() > 100000)
    {
        std::println("[POST - API NEW] INVALID Request is too large.");
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
        std::println("[POST - API NEW] INVALID Empty paste text body.");
        res.status = httplib::StatusCode::BadRequest_400;
        res.set_content("Request Invalid! No Text...", "text/plain");
        return;
    }

    // Generate random code
    const std::string uniqueCode = std::format("{}", sharepaste::generateRandomString(sharepaste::uniqueCodeLength));

    bool insert_success = sharepaste::G_DATABASE.insertPaste(uniqueCode, pasteBody.value(), std::nullopt, std::nullopt);
    if (!insert_success)
    {
        std::println("[POST - API NEW] Insert Failed.");
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
    std::println("[GET - Paste Data] Recieved.");
    std::println("{}", sharepaste::getReqClientInfoString(req));

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
    std::println("[GET - Paste Data] Fetching Data.");
    std::optional<PasteData> retrievedPaste = sharepaste::G_DATABASE.getPasteData(uniqueCode);

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
    std::println("[GET - Webpage] Sending Static Page");
    std::println("{}", sharepaste::getReqClientInfoString(req));

    // serves script.js and style.css that are statically mounted at /www.
    res.set_file_content("./www/index.html", "text/html");
}


int main(int argc, char* argv[])
{
    if (argc > 1) // Check CLI arguements
    {
        if (strcmp(argv[1], "--test") == 0)
        {
            std::println("[START] Running --tests");
            sharepaste::runTests();
            exit(0);
        }
    }

    std::println("[START] Beginning SharePaste");

    httplib::Server svr;

    const std::string database_subfolder = "data";
    const std::string database_filename = "sharepaste.db";

    sharepaste::G_DATABASE.connect(sharepaste::databasePathConstructor(database_subfolder, database_filename));

    std::println("[Create Table] Creating table");
    sharepaste::G_DATABASE.createPasteTable();

    std::println("[Register] Adding get /api/new handler");
    svr.Post("/api/new", postRequestAPINewPaste);

    std::println("[Register] Adding get /api/find handler");
    svr.Get("/api/find", getRequestPasteData);

    auto ret = svr.set_mount_point("/www", "./www");
    if (!ret)
    {
        std::println("Cant mount /www to ./www");
    }

    std::println("[Register] Adding get /* handler");
    svr.Get(R"(.*)", getPasteWebpage);

    std::string host  = "0.0.0.0";
    int port = 8080;

    std::println("[Info] Attempting to listen on {}:{}", host, port);

    if (!svr.listen(host, port)) {
        std::cerr << "[ERROR] Failed to bind to " << host << ":" << port
                  << ". Maybe another program is using it?\n";
        return -1;
    }
}