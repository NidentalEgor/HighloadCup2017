#ifndef HTTP_SERVER_H_INCLUDED
#define HTTP_SERVER_H_INCLUDED
#pragma once

#include <string>

#include "../Utils/Traceable.h"
#include "RequestProcessor.h"

class HttpServer
        : public Traceable
{
public:
    enum class LoadedDataType
    {
        Zipped = 0,
        Unzipped
    };

    HttpServer(
            const std::string& ip_address,
            const uint16_t port,
            const LoadedDataType loaded_data_type,
            const std::string& loaded_data_path,
            const size_t threads_count);

    void Run();

private:
    std::string ip_address_;
    uint16_t port_;
    uint16_t threads_count_;
    std::shared_ptr<RequestProcessor> request_processor_;
};

#endif // HTTP_SERVER_H_INCLUDED