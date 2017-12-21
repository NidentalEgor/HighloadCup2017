#ifndef REQUEST_PROCESSOR_H_INCLUDED
#define REQUEST_PROCESSOR_H_INCLUDED
#pragma once

#include <memory>
#include <string>

#include "../Utils/Traceable.h"
#include "../HttpParser/HttpParser.h"
#include "../DataStorage/DataStorage.h"

class RequestProcessor
    : public Traceable
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
    RequestProcessor(
            const std::shared_ptr<DataStorage>& data_storage);

    RequestProcessingResult ProcessRequest(
            const char* message,
            const size_t message_size) const;

private:
    template <typename T>        
    RequestProcessingResult ProcessGetEntityByIdRequest(
            const Id entity_id) const;

    RequestProcessingResult ProcessAverageLocationMarkRequest(
            const HttpParser& http_parser) const;

    DataStorage::GetAverageLocationMarkQuery FillAverageLocationMarkQuery(
            const HttpParser& http_parser) const;
    
    RequestProcessingResult ProcessGetVisitsByUserIdRequest(
            const HttpParser& http_parser) const;

    DataStorage::GetVisistsByUserIdQuery FillVisistsByUserIdQuery(
            const HttpParser& http_parser) const;

    std::pair<bool, std::unique_ptr<User>> FillUserToUpdate(
            HttpParser& http_parser) const;

    std::pair<bool, std::unique_ptr<Location>> FillLocationToUpdate(
            HttpParser& http_parser) const;

    std::pair<bool, std::unique_ptr<Visit>> FillVisitToUpdate(
            HttpParser& http_parser) const;

    template<typename T>
    RequestProcessingResult ProcessUpdateEntityRequest(
            HttpParser& http_parser) const;

    template<typename T>
    std::pair<bool, std::unique_ptr<T>> FillEntityToUpdate(
            HttpParser& http_parser) const;

    template<typename T>
    RequestProcessingResult ProcessAddEntityRequest(
            HttpParser& http_parser) const;

private:
    std::shared_ptr<DataStorage> data_storage_;
};

#endif // REQUEST_PROCESSOR_H_INCLUDED