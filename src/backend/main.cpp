#include <string>
#include <string_view>
#include <optional>
#include <thread>
#include <chrono>

#include <sqlite3.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <dbmanager.h>
#include <utility.h>
#include <testtools.h>
#include <ratelimiter.h>

using json = nlohmann::json;

namespace sharepaste
{
    namespace env
    {
        std::vector<std::string> trustedProxy = stringToSplitArray(fetchEnv("SP_TrustedProxies")); // format like 192.168.0.1, 192.168.0.2, 192.168.0.92
        const int tokenCapacity = fetchEnvInt("SP_RateLimit_TokenCapacity", 20);                   // format with an int like 10 and set to 0 to disable rate limiting
        const double tokenRefillRate = fetchEnvDouble("SP_RateLimit_RefillRate", .5);              // format with an double like .5

        const int blockAttemptWindow = fetchEnvInt("SP_RateLimit_BlockAttemptWindow", 5); // format with an int like 5 - minutes
        const int blockMaxAttempts = fetchEnvInt("SP_RateLimit_BlockMaxAttempts", 10);    // format with an int like 10 - attempts
        const int blockDuration = fetchEnvInt("SP_RateLimit_BlockDuration", 10);          // format with an int like 10 and set to 0 to disable long blocks - minutes

        const std::chrono::seconds cleanUpInterval = std::chrono::seconds(fetchEnvInt("SP_RateLimit_CleanUpInterval", 600)); // how often to run ratelimit memory clean up. - seconds
        const int cleanUpMinimumAge = fetchEnvInt("SP_RateLimit_CleanMinimumAge", 5);                                        // how old should the ips last check for them to be considered for removal. - minutes

        const std::string adminEmail = fetchEnv("SP_AdminContactEmail"); // admin email to fill out about page

        const int maxPasteSize = fetchEnvInt("SP_RateLimit_MaxPasteSize", 1000000); // character limit for backend to disregard request.
    } // env

    managerSQL G_DATABASE;
    IpRateLimiter G_RATELIMITER(env::tokenCapacity, env::tokenRefillRate, env::blockAttemptWindow, env::blockDuration, env::blockMaxAttempts);
    inline constexpr int uniqueCodeLength{15}; // Roughly 3,527,930,788,646,880 possiblities, chance of a conflict is slim and if it does happen just have the user try the request again ez pz.

} // sharepaste

void addSecurityHeaders(httplib::Response &res)
{
    res.set_header("Strict-Transport-Security", "max-age=31536000; includeSubDomains");

    res.set_header("X-Frame-Options", "DENY");

    res.set_header("X-Content-Type-Options", "nosniff");
    res.set_header("server", "server");
    res.set_header("Referrer-Policy", "strict-origin-when-cross-origin");
    res.set_header("Content-Security-Policy", "default-src 'self'; style-src 'self' 'unsafe-inline'; script-src 'self'; object-src 'none';");
    // block these
    res.set_header("Permissions-Policy", "camera=(), microphone=(), geolocation=()");
} // addSecurityHeaders

void postRequestAPINewPaste(const httplib::Request &req, httplib::Response &res) // set up some sort of rate limiting
{
    // sharepaste::printLine("[POST - API NEW] Recieved.");

    // Check for invalid post request
    if (!req.has_header("Content-Length") || req.body.empty())
    {
        auto val = req.get_header_value("Content-Length");

        sharepaste::printLine("[POST - API NEW] INVALID Request has issues.");
        res.set_content("Request Invalid! Malformed", "text/plain");
        return;
    }

    // Check for crazy large payload
    if (req.body.size() > sharepaste::env::maxPasteSize)
    {
        sharepaste::printLine("[POST - API NEW] INVALID Request is too large.");
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
        sharepaste::printLine("[POST - API NEW] INVALID Empty paste text body.");
        res.status = httplib::StatusCode::BadRequest_400;
        res.set_content("Request Invalid! No Text...", "text/plain");
        return;
    }

    // Generate random code
    const std::string uniqueCode = std::format("{}", sharepaste::generateRandomString(sharepaste::uniqueCodeLength));

    bool insert_success = sharepaste::G_DATABASE.insertPaste(uniqueCode, pasteBody.value(), std::nullopt, std::nullopt);
    if (!insert_success)
    {
        sharepaste::printLine("[POST - API NEW] Insert Failed.");
        res.status = httplib::StatusCode::InternalServerError_500;
        res.set_content("Request Invalid! Server Failed", "text/plain");
        return;
    }

    // if nothing returned early then respond with the sharelink
    res.set_content(uniqueCode, "text/plain");
    sharepaste::printLine("[POST - API NEW] New Paste Entry - {}", uniqueCode);
} // postRequestAPINewPaste

void getHealthStatus(const httplib::Request &req, httplib::Response &res)
{
    res.set_content("Alive.", "text/plain");
}

void getRequestPasteData(const httplib::Request &req, httplib::Response &res)
{
    // sharepaste::printLine("[GET - Paste Data] Recieved.");

    std::string uniqueCode{"NO CODE PROVIDED"};

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
    sharepaste::printLine("[GET - Paste Data] Fetching Data.");
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
    sharepaste::printLine("[GET - Paste Data] JSON - {}", responsePayload.dump());
    res.set_content(responsePayload.dump(), "text/json");
} // getRequestPasteData

void getPasteWebpage(const httplib::Request &req, httplib::Response &res)
{
    // sharepaste::printLine("[GET - Webpage] Sending Home Page");

    // serves script.js and style.css that are statically mounted at /www.
    res.set_file_content("./www/index.html", "text/html");
} // getPasteWebpage

void getAboutWebpage(const httplib::Request &req, httplib::Response &res)
{
    // sharepaste::printLine("[GET - Webpage] Sending About Page");

    // serves script.js and style.css that are statically mounted at /www.
    res.set_file_content("./www/about.html", "text/html");
} // getAboutWebpage

void getApiEmail(const httplib::Request &req, httplib::Response &res)
{
    // sharepaste::printLine("[GET - Webpage] Sending About Page");
    // "admin@email"
    res.set_content(sharepaste::env::adminEmail, "text/plain");
    // res.set_content("admin@email", "text/plain");
} // getApiEmail

void getApiMaxPasteSize(const httplib::Request &req, httplib::Response &res)
{
    res.set_content(std::to_string(sharepaste::env::maxPasteSize), "text/plain");
}

void getDrop404Request(const httplib::Request &req, httplib::Response &res)
{
    res.status = 404;
    res.set_header("Connection", "close");
} // getDrop404Request

httplib::Server::HandlerResponse preRequestHandlerRateLimit(const httplib::Request &req, httplib::Response &res)
{
    if (req.path == "/.well-known/appspecific/com.chrome.devtools.json")
    {
        return httplib::Server::HandlerResponse::Handled;
    }

    auto clientInfo = sharepaste::getReqClientInfoParse(req);

    // print client info
    sharepaste::printLine("{} with this many Tokens {}, are they blocked? |{}| and for how long left {}s.",
                          sharepaste::getReqClientInfoString(req), sharepaste::G_RATELIMITER.checkTokens(clientInfo.ip),
                          sharepaste::G_RATELIMITER.isBlocked(clientInfo.ip), sharepaste::G_RATELIMITER.blockTimeLeft(clientInfo.ip).count());
    int tokenCost = 1;
    if (req.matched_route == "/api/new")
        tokenCost = 2;

    addSecurityHeaders(res);

    // rate limit
    if (!sharepaste::G_RATELIMITER.allowRequest((clientInfo.ip), tokenCost))
    {
        res.status = 429;
        res.set_header("Connection", "close");
        return httplib::Server::HandlerResponse::Handled;
    }

    return httplib::Server::HandlerResponse::Unhandled;
} // preRequestHandlerRateLimit

void rateLimitCleanUpThread(std::chrono::seconds loopDuration)
{
    while (1)
    {
        if (sharepaste::G_RATELIMITER.size() > 0)
        {
            sharepaste::printLine("[Clean RateLimit] {} IP/s in memory.", sharepaste::G_RATELIMITER.size());
            sharepaste::G_RATELIMITER.cleanAll(sharepaste::env::cleanUpMinimumAge); // minutes
            sharepaste::printLine("[Clean RateLimit] {} IP/s after clean.", sharepaste::G_RATELIMITER.size());
            // sharepaste::G_RATELIMITER.printAllIps();
        }
        std::this_thread::sleep_for(std::max(loopDuration, std::chrono::seconds(1)));
    }
} // rateLimitCleanUpThread

int main(int argc, char *argv[])
{
    if (argc > 1) // Check CLI arguements
    {
        if (strcmp(argv[1], "--test") == 0)
        {
            sharepaste::printLine("[START] Running --tests");
            sharepaste::runTests();
            exit(0);
        }
    }

    sharepaste::printLine("[START] Beginning SharePaste");

    httplib::Server svr;

    // setting local proxy
    if (!sharepaste::env::trustedProxy.empty())
    {
        svr.set_trusted_proxies(sharepaste::env::trustedProxy);
        std::for_each(sharepaste::env::trustedProxy.cbegin(), sharepaste::env::trustedProxy.cend(),
                      [](std::string_view n)
                      { sharepaste::printLine("[Trusted Proxy] Set: {}", n); });
    }
    else
        sharepaste::printLine("[Trusted Proxy] None Set...");

    // setting up local db
    const std::string database_subfolder = "data";
    const std::string database_filename = "sharepaste.db";
    sharepaste::G_DATABASE.connect(sharepaste::databasePathConstructor(database_subfolder, database_filename));

    sharepaste::printLine("[Create Table] Creating table");
    sharepaste::G_DATABASE.createPasteTable();
    sharepaste::G_DATABASE.addColumnIfNotExists("is_wrapped", "BOOLEAN NOT NULL DEFAULT FALSE");

    // pre-request
    svr.set_pre_request_handler(preRequestHandlerRateLimit);

    // setting up handles
    sharepaste::printLine("[Register] Adding get /api/new handler");
    svr.Post("/api/new", postRequestAPINewPaste);

    sharepaste::printLine("[Register] Adding get /api/find handler");
    svr.Get("/api/find", getRequestPasteData);

    sharepaste::printLine("[Register] Adding get /api/email handler");
    svr.Get("/api/email", getApiEmail);

    sharepaste::printLine("[Register] Adding get /api/maxsize handler");
    svr.Get("/api/maxsize", getApiMaxPasteSize);

    // mounts www folder so js, html, css can be accessed via /www/something.sm without invidiual handlers
    auto ret = svr.set_mount_point("/www", "./www");
    if (!ret)
    {
        sharepaste::printLine("Cant mount /www to ./www");
        exit(-1);
    }

    sharepaste::printLine("[Register] Adding get /about handler");
    svr.Get("/about", getAboutWebpage);

    sharepaste::printLine("[Register] Common paths to 404.");
    svr.Get("/security.txt", getDrop404Request);
    svr.Get("/robots.txt", getDrop404Request);

    sharepaste::printLine("[Register] Alive endpoint");
    svr.Get("/status", getHealthStatus);

    sharepaste::printLine("[Register] Adding get /* handler");
    svr.Get(R"(.*)", getPasteWebpage);

    // default host/port
    std::string host = "0.0.0.0";
    int port = 8080;

    // create ratelimit cleaning cycle
    std::thread rateLimitCleaner(rateLimitCleanUpThread, sharepaste::env::cleanUpInterval);
    rateLimitCleaner.detach();

    // binds to network
    sharepaste::printLine("[Info] Attempting to listen on {}:{}", host, port);

    if (!svr.listen(host, port))
    {
        std::cerr << "[ERROR] Failed to bind to " << host << ":" << port
                  << ". Maybe another program is using it?\n";
        return -1;
    }
}
