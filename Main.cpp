#include <iostream>

#include "HttpServer/HttpServer.h"
#include "DataStorage/DataStorage.h"
#include "HttpParser/HttpParser.h"

int main(int argc, char* argv[])
{
    try
    {
        // For docker
        // Network::EchoServer Srv("0.0.0.0", "80", 1);
        // For docker

        // Local network test
        // Network::EchoServer Srv("192.168.2.103", "5555", 4);
        // Local network test


        
        // Testing
        // data_storage_->LoadZippedData("/tmp/data/data.zip");
        // data_storage_->LoadZippedData("/HighloadCup2017/data.zip");
        // data_storage_->LoadZippedData("/home/egor/Repositories/highload_data_zip/data.zip");
        // data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/TRAIN/data/");
        // data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/FULL/data/");

        Network::EchoServer server(
                "127.0.0.1",
                "5555",
                Network::LoadedDataType::Unzipped,
                // "/home/egor/Repositories/highload_data_zip/data.zip",
                // "/home/egor/Repositories/hlcupdocs/data/FULL/data/",
                "/home/egor/Repositories/hlcupdocs/data/TRAIN/data/",
                4);
        // Testing

        std::cin.get();
    }
    catch (std::exception const &e)
    {
        std::cout << "catch (std::exception const &e)" << std::endl;
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
