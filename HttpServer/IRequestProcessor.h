#ifndef I_REQUEST_PROCESSOR_H_INCLUDED
#define I_REQUEST_PROCESSOR_H_INCLUDED
#pragma once

class IRequestProcessor
{
public:
    enum class RequestProcessingStatus
    {
        Ok = 200,
        NotFound = 404,
        BadRequest = 400,
        HttpParserUnknownRequestType = 10000
    };

    using RequestProcessingResult =
            std::pair<RequestProcessingStatus, std::unique_ptr<std::string>>;

public:
    virtual ~IRequestProcessor()
    {
    };

    virtual RequestProcessingResult ProcessRequest(
            const char* message,
            const size_t message_size) const = 0;
};

#endif // I_REQUEST_PROCESSOR_H_INCLUDED