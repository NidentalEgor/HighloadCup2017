#include <iostream>

#include "HttpServer.h"
#include "DataStorage.h"
#include "HttpParser.h"
#include "DataExtracter.h"

int main(int argc, char* argv[])
{
    try
    {
        DataExtracter data_extracter;
        data_extracter.ExtractData(
                "/tmp/data/data.zip",
                "/tmp/data");
        // For docker
        // Network::EchoServer Srv("0.0.0.0", "80", 4);
        // For docker

        // Network::EchoServer Srv("192.168.2.103", "5555", 4);
        Network::EchoServer Srv("127.0.0.1", "5555", 4);
        
        std::cin.get();
    }
    catch (std::exception const &e)
    {
        std::cout << "catch (std::exception const &e)" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

// int main(int argc, char* argv[])
// {
    // std::string request =
    //         R"(GET /locations/3558 HTTP/1.1
    //             Host: travels.com
    //             User-Agent: Technolab/1.0 (Docker; CentOS) Highload/1.0
    //             Accept: */*
    //             Connection: keep-alive)";

    // std::string request =
    //         R"(POST /users/8230?query_id=0 HTTP/1.1 Host: travels.com User-Agent: Technolab/1.0 (Docker; CentOS) Highload/1.0 Accept: */* Connection: close Content-Length: 84 Content-Type: application/json {"birth_date": 47779200, "first_name": "\u0410\u0440\u043a\u0430\u0434\u0438\u0439"})";

    // std::string request = R"(POST /users/new?query_id=13787 HTTP/1.1
    //     Host: travels.com
    //     User-Agent: Technolab/1.0 (Docker; CentOS) Highload/1.0
    //     Accept: */*
    //     Connection: close
    //     Content-Length: 212
    //     Content-Type: application/json
        
    //     {"first_name": "\u041a\u0438\u0440\u0438\u043b\u043b", "last_name": "\u041b\u0435\u0431\u044b\u043a\u0430\u0432\u0435\u043d", "gender": "m", "id": 10058, "birth_date": -478569600, "email": "ohrisnaunhos@list.me"}
    //     )";

    // std::string request = R"(POST /users/new?query_id=13787 HTTP/1.1 Host: travels.com)";

    // HttpParser http_parser;
    // http_parser.ParseHttpRequest(
    //         const_cast<char*>(request.c_str()),
    //         request.size());

    // return 0;
// }
