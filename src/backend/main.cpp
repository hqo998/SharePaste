#include <iostream>
#include <sqlite3.h>
#include <httplib.h>

void getRequestAPI(const httplib::Request &, httplib::Response &res)
{
    res.set_content("Hello world!", "text/plain");
}

void serveFrontEnd()
{
    
}

int main()
{
    std::cout << "Hello, World!" << std::endl;
    
    httplib::Server svr;

    svr.Get("/api", getRequestAPI);

    auto ret = svr.set_mount_point("/", "./www");
    if (!ret)
    {
        std::cerr << "Frontend folder not found!" << '\n';
    }

    svr.listen("0.0.0.0", 80);
}