#include <iostream>

#include "HttpServer/HttpServer.h"

int main()
{
    try
    {
        HttpServer http_server(
                "127.0.0.1",
                1222,
                HttpServer::LoadedDataType::Zipped,
                // HttpServer::LoadedDataType::Unzipped,
                // "/tmp/data/data.zip",
                // "/home/egor/Repositories/highload_data_zip/data_train_good.zip",
                // "/home/egor/Repositories/hlcupdocs/data/FULL/data/",
                "/home/egor/Repositories/highload_data_zip/data_full.zip",
                4);

        http_server.Run();
    }
    catch (std::exception const &e)
    {
        std::cout << "catch (std::exception const &e)" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

// #include "HttpServer/HttpServerBoost.h"
// #include "DataStorage/DataStorage.h"
// #include "HttpParser/HttpParser.h"

// int main(int argc, char* argv[])
// {
//     try
//     {
//         // For docker
//         Network::EchoServer server(
//                 "0.0.0.0",
//                 "80",
//                 Network::LoadedDataType::Zipped,
//                 "/tmp/data/data.zip",
//                 3);
//         // For docker

//         // Local network test
//         // Network::EchoServer Srv("192.168.2.103", "5555", 4);
//         // Local network test

//         // Testing
//         // data_storage_->LoadZippedData("/tmp/data/data.zip");
//         // data_storage_->LoadZippedData("/HighloadCup2017/data.zip");
//         // data_storage_->LoadZippedData("/home/egor/Repositories/highload_data_zip/data.zip");
//         // data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/TRAIN/data/");
//         // data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/FULL/data/");

//         // Network::EchoServer server(
//         //         "127.0.0.1",
//         //         "5555",
//         //         // Network::LoadedDataType::Unzipped,
//         //         Network::LoadedDataType::Zipped,
//         //         // "/home/egor/Repositories/highload_data_zip/data.zip",
//         //         "/tmp/data/data.zip",
//         //         // "/home/egor/Repositories/hlcupdocs/data/FULL/data/",
//         //         // "/home/egor/Repositories/hlcupdocs/data/TRAIN/data/",
//         //         4);
//         // Testing

//         std::cin.get();
//     }
//     catch (std::exception const &e)
//     {
//         std::cout << "catch (std::exception const &e)" << std::endl;
//         std::cerr << e.what() << std::endl;
//     }

//     return 0;
// }


// int main(int argc, char* argv[])
// {
//   try
//   {
//     if (argc != 2)
//     {
//       std::cerr << "Usage: async_tcp_echo_server <port>\n";
//       return 1;
//     }

//     boost::asio::io_service io_service;

//     server s(io_service, std::atoi(argv[1]));

//     io_service.run();
//   }
//   catch (std::exception& e)
//   {
//     std::cerr << "Exception: " << e.what() << "\n";
//   }

//   return 0;
// }
