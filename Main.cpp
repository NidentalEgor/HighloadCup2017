#include "HttpServer.h"
#include "DataStorage.h"
#include <iostream>

int main(int argc, char* argv[])
{
    // HttpServer http_server;
    // http_server.Run();
    // DataStorage data_storage;
    // data_storage.LoadData(
    //         //"/home/egor/Repositories/hlcupdocs/data/TRAIN/data/");
    //         "/home/egor/Repositories/HighloadCup2017/Tests/DataStorageTests/TestData/ReadData/");
    // data_storage.DumpData();

    try
    {
        Network::EchoServer Srv("127.0.0.1", "8181", 4);
        // Network::EchoServer Srv("192.168.2.103", "5555", 4);
        
        std::cin.get();
    }
    catch (std::exception const &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}