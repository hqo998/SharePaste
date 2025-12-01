#include <iostream>
#include <sqlite3.h>
#include <httplib.h>



int main() {
    std::cout << "Hello, World!" << std::endl;
    
    httplib::Server svr;

    svr.Get("/api", [](const httplib::Request &, httplib::Response &res){
    res.set_content("Hello world!", "text/plain");
    });

    svr.listen("0.0.0.0", 8080);
}