#ifndef REQUEST_PROCESSOR_H_INCLUDED
#define REQUEST_PROCESSOR_H_INCLUDED
#pragma once

#include <memory>
#include <string>

#include "../Utils/Traceable.h"
#include "../HttpParser/HttpParser.h"
#include "../DataStorage/IDataStorage.h"
#include "IRequestProcessor.h"

class RequestProcessor
        : public IRequestProcessor
        , public Traceable
{
public:
    RequestProcessor(
            const std::shared_ptr<IDataStorage>& data_storage);

    RequestProcessingResult ProcessRequest(
            const char* message,
            const size_t message_size) const override;

private:
    template <typename T>        
    RequestProcessingResult ProcessGetEntityByIdRequest(
            const Id entity_id) const;

    RequestProcessingResult ProcessAverageLocationMarkRequest(
            const HttpParser& http_parser) const;

    IDataStorage::GetAverageLocationMarkQuery FillAverageLocationMarkQuery(
            const HttpParser& http_parser) const;
    
    RequestProcessingResult ProcessGetVisitsByUserIdRequest(
            const HttpParser& http_parser) const;

    IDataStorage::GetVisistsByUserIdQuery FillVisistsByUserIdQuery(
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
//
    template <typename T>
    RequestProcessingResult CommonGetEntity(
            T&& entity) const;
//

private:
    std::shared_ptr<IDataStorage> data_storage_;
};

#endif // REQUEST_PROCESSOR_H_INCLUDED