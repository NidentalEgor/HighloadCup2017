// #include "HttpServer.h"
#include "DataStorage.h"

int main(int argc, char* argv[])
{
    // HttpServer http_server;
    // http_server.Run();

    // std::multimap<>

    DataStorage data_storage;
    data_storage.LoadData(
            //"/home/egor/Repositories/hlcupdocs/data/TRAIN/data/");
            "/home/egor/Repositories/HighloadCup2017/Tests/DataStorageTests/TestData/ReadData/");
    data_storage.DumpData();
    return 0;
}