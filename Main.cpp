#include "HttpServer.h"
#include "DataStorage.h"

int main(int argc, char* argv[])
{
    // HttpServer http_server;
    // http_server.Run();

    DataStorage data_storage;
    data_storage.LoadData(
            "/home/egor/Repositories/hlcupdocs/data/TRAIN/data/");
    
    return 0;
}