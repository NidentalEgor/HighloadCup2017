#include "../Utils/Macroses.h"
#include "../HttpParser/HttpParser.h"
#include "../DataStorage/Entities/EntitiesValidator.h"
#include "RequestProcessor.h"

static const std::string not_empty_ok_response(
        "HTTP/1.1 200 OK\r\n"
        "S: b\r\n"
        "C: k\r\n"
        "B: a\r\n"
        "Content-Length: {}\r\n"
        "\r\n"
        "{}");

static const std::string empty_ok_response(
        "HTTP/1.1 200 OK\r\n"
        "S: b\r\n"
        "C: k\r\n"
        "B: a\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "{}");

static const std::string not_found_response(
        "HTTP/1.1 404 Not Found\r\n"
        "S: b\r\n"
        "C: k\r\n"
        "B: a\r\n"
        "Content-Length: 0\r\n"
        "\r\n");

static const std::string bad_data_response(
        "HTTP/1.1 400 Bad Request\r\n"
        "S: b\r\n"
        "C: k\r\n"
        "B: a\r\n"
        "Content-Length: 0\r\n"
        "\r\n");

RequestProcessor::RequestProcessor(
        const std::shared_ptr<DataStorage>& data_storage)
    : data_storage_(data_storage)
{
}

RequestProcessor::RequestProcessingResult RequestProcessor::ProcessRequest(
        const char* message,
        const size_t message_size) const
{
    // ENSURE_TRUE_OTHERWISE_RETURN(
    //         http_parser.ParseHttpRequest(
    //             const_cast<char*>(message),
    //             message_size),
    //         std::make_pair(
    //             RequestProcessingStatus::BadRequest,
    //             nullptr));

    DebugTraceCharacters(
            const_cast<char*>(message),
            message_size);

    HttpParser http_parser;
    const auto parse_http_request_result =
            http_parser.ParseHttpRequest(
                const_cast<char*>(message),
                message_size);
    
    ENSURE_TRUE_OTHERWISE_RETURN(
            parse_http_request_result != HttpParser::ErrorType::ErrorTypeNotFound,
            std::make_pair(
                RequestProcessingStatus::NotFound,
                std::make_unique<std::string>(
                    not_found_response)));

    ENSURE_TRUE_OTHERWISE_RETURN(
            parse_http_request_result != HttpParser::ErrorType::ErrorTypeBadRequest,
            std::make_pair(
                RequestProcessingStatus::BadRequest,
                std::make_unique<std::string>(
                    bad_data_response)));

    switch (http_parser.GetRequestType())
    {
        case HttpParser::RequestType::GetUserById:
        {
            return ProcessGetEntityByIdRequest<User>(
                    http_parser.GetEntityId());
        } break;

        case HttpParser::RequestType::GetLocationById:
        {
            return ProcessGetEntityByIdRequest<Location>(
                    http_parser.GetEntityId());
        } break;

        case HttpParser::RequestType::GetVisitById:
        {
            return ProcessGetEntityByIdRequest<Visit>(
                    http_parser.GetEntityId());
        } break;

        case HttpParser::RequestType::GetAverageLocationMark:
        {
            return ProcessAverageLocationMarkRequest(
                    http_parser);
        } break;

        case HttpParser::RequestType::GetVisitsByUserId:
        {
            return ProcessGetVisitsByUserIdRequest(
                    http_parser);
        } break;
                
        case HttpParser::RequestType::UpdateUserById:
        {
            return ProcessUpdateEntityRequest<User>(http_parser);
        } break;

        case HttpParser::RequestType::UpdateVisitById:
        {
            return ProcessUpdateEntityRequest<Visit>(http_parser);
        } break;

        case HttpParser::RequestType::UpdateLocationById:
        {
            return ProcessUpdateEntityRequest<Location>(http_parser);
        } break;

        case HttpParser::RequestType::AddUser:
        {
            return ProcessAddEntityRequest<User>(http_parser);
        } break;

        case HttpParser::RequestType::AddLocation:
        {
            return ProcessAddEntityRequest<Location>(http_parser);
        } break;

        case HttpParser::RequestType::AddVisit:
        {
            return ProcessAddEntityRequest<Visit>(http_parser);
        } break;
    }

    return std::make_pair(
                RequestProcessingStatus::HttpParserUnknownRequestType,
                nullptr);
}

template <typename T>        
RequestProcessor::RequestProcessingResult RequestProcessor::ProcessGetEntityByIdRequest(
        const Id entity_id) const
{
    const auto entity =
            data_storage_->GetEntityById<T>(entity_id);

    ENSURE_TRUE_OTHERWISE_RETURN(
            entity != nullptr,
            std::make_pair(
                RequestProcessingStatus::NotFound,
                std::make_unique<std::string>(
                    not_found_response)));

    return std::make_pair(
            RequestProcessingStatus::Ok,
            std::make_unique<std::string>(
                fmt::format(
                    not_empty_ok_response,
                    entity->size(),
                    *entity)));
}

RequestProcessor::RequestProcessingResult RequestProcessor::ProcessAverageLocationMarkRequest(
        const HttpParser& http_parser) const
{
    const auto query =
            FillAverageLocationMarkQuery(
                http_parser);

    const auto average_location_mark =
            data_storage_->GetAverageLocationMark(query);
    
    ENSURE_TRUE_OTHERWISE_RETURN(
            average_location_mark,
            std::make_pair(
                RequestProcessingStatus::NotFound,
                std::make_unique<std::string>(
                    not_found_response)));

    return std::make_pair(
            RequestProcessingStatus::Ok,
            std::make_unique<std::string>(
                fmt::format(
                    not_empty_ok_response,
                    average_location_mark->size(),
                    *average_location_mark)));
}

DataStorage::GetAverageLocationMarkQuery RequestProcessor::FillAverageLocationMarkQuery(
        const HttpParser& http_parser) const
{
    DataStorage::GetAverageLocationMarkQuery query(http_parser.GetEntityId());

    const int mask =
            http_parser.GetAdditionalInfoMask();
    if (mask & static_cast<int>(HttpParser::HttpParserFlags::FromDateFlag))
    {
        query.from_date = http_parser.GetFromDate();
    }

    if (mask & static_cast<int>(HttpParser::HttpParserFlags::ToDateFlag))
    {
        query.to_date = http_parser.GetToDate();
    }

    if (mask & static_cast<int>(HttpParser::HttpParserFlags::FromAgeFlag))
    {
        query.from_age = http_parser.GetFromAge();
    }

    if (mask & static_cast<int>(HttpParser::HttpParserFlags::ToAgeFlag))
    {
        query.to_age = http_parser.GetToAge();
    }

    if (mask & static_cast<int>(HttpParser::HttpParserFlags::GenderFlag))
    {
        query.gender = http_parser.GetGender();
    }

    return query;
}

RequestProcessor::RequestProcessingResult RequestProcessor::ProcessGetVisitsByUserIdRequest(
        const HttpParser& http_parser) const
{
    const auto query =
            FillVisistsByUserIdQuery(http_parser);

    const auto visits =
            data_storage_->GetVisistsByUserId(query);

    ENSURE_TRUE_OTHERWISE_RETURN(
            visits != nullptr,
            std::make_pair(
                RequestProcessingStatus::NotFound,
                std::make_unique<std::string>(
                    not_found_response)));
    
    return std::make_pair(
            RequestProcessingStatus::Ok,
            std::make_unique<std::string>(
                fmt::format(
                    not_empty_ok_response,
                    visits->size(),
                    *visits)));
}

DataStorage::GetVisistsByUserIdQuery RequestProcessor::FillVisistsByUserIdQuery(
        const HttpParser& http_parser) const
{
    DataStorage::GetVisistsByUserIdQuery query(http_parser.GetEntityId());

    const int mask =
            http_parser.GetAdditionalInfoMask();
    if (mask & static_cast<int>(HttpParser::HttpParserFlags::FromDateFlag))
    {
        query.from_date = http_parser.GetFromDate();
    }

    if (mask & static_cast<int>(HttpParser::HttpParserFlags::ToDateFlag))
    {
        query.to_date = http_parser.GetToDate();
    }

    if (mask & static_cast<int>(HttpParser::HttpParserFlags::CountryFlag))
    {
        query.country = http_parser.GetCountry();
    }

    if (mask & static_cast<int>(HttpParser::HttpParserFlags::ToDistanceFlag))
    {
        query.to_distance = http_parser.GetToDistance();
    }

    return query;
}

template<typename T>
std::pair<bool, std::unique_ptr<T>> RequestProcessor::FillEntityToUpdate(
        HttpParser& http_parser) const
{
    // Compile time error for unknown type.
}

template<>
std::pair<bool, std::unique_ptr<User>> RequestProcessor::FillEntityToUpdate<User>(
        HttpParser& http_parser) const
{
    return FillUserToUpdate(http_parser);
}

template<>
std::pair<bool, std::unique_ptr<Location>> RequestProcessor::FillEntityToUpdate<Location>(
        HttpParser& http_parser) const
{
    return FillLocationToUpdate(http_parser);
}

template<>
std::pair<bool, std::unique_ptr<Visit>> RequestProcessor::FillEntityToUpdate<Visit>(
        HttpParser& http_parser) const
{
    return FillVisitToUpdate(http_parser);
}

template<typename T>
RequestProcessor::RequestProcessingResult RequestProcessor::ProcessUpdateEntityRequest(
        HttpParser& http_parser) const
{
    const auto entity_to_update_filling_result =
            FillEntityToUpdate<T>(http_parser);

    ENSURE_TRUE_OTHERWISE_RETURN(
            entity_to_update_filling_result.first,
            std::make_pair(
                RequestProcessingStatus::BadRequest,
                std::make_unique<std::string>(
                    bad_data_response)));

    ENSURE_TRUE_OTHERWISE_RETURN(
            data_storage_->UpdateEntity<T>(*entity_to_update_filling_result.second) ==
                DataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated,
            std::make_pair(
                RequestProcessingStatus::NotFound,
                std::make_unique<std::string>(
                    not_found_response)));

    return std::make_pair(
            RequestProcessingStatus::Ok,
            std::make_unique<std::string>(empty_ok_response));
}

std::pair<bool, std::unique_ptr<User>> RequestProcessor::FillUserToUpdate(
        HttpParser& http_parser) const
{
    User user_to_update;
    user_to_update.id = http_parser.GetEntityId();

    const auto fields_to_update =
            http_parser.GetBodyContent();

    EntitiesValidator entities_validator;
    ENSURE_TRUE_OTHERWISE_RETURN(
            entities_validator.UserDataForUpdate(fields_to_update),
                std::make_pair(
                    false,
                    nullptr));

    const auto mask = entities_validator.GetMask();
            
    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Email))
    {
        user_to_update.email =
                entities_validator.GetStringAttribute(
                    EntitiesValidator::EntityAttributes::Email);
    }
                            
    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::FirstName))
    {
        user_to_update.first_name =
                entities_validator.GetStringAttribute(
                    EntitiesValidator::EntityAttributes::FirstName);
    }
                        
    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::LastName))
    {
        user_to_update.last_name =
                entities_validator.GetStringAttribute(
                    EntitiesValidator::EntityAttributes::LastName);
    }
                            
    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Gender))
    {
        user_to_update.gender =
                entities_validator.GetStringAttribute(
                    EntitiesValidator::EntityAttributes::Gender) == "m"
                    ? Gender::Male
                    : Gender::Female;
    }
                            
    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::BirthDate))
    {
        user_to_update.birth_date =
                entities_validator.GetInt64Attribute(
                    EntitiesValidator::EntityAttributes::BirthDate);
    }
    
    return std::make_pair(
            true,
            std::make_unique<User>(user_to_update));
}

std::pair<bool, std::unique_ptr<Location>> RequestProcessor::FillLocationToUpdate(
        HttpParser& http_parser) const
{
    Location location_to_update;
    location_to_update.id = http_parser.GetEntityId();

    const auto fields_to_update =
            http_parser.GetBodyContent();

    EntitiesValidator entities_validator;
    ENSURE_TRUE_OTHERWISE_RETURN(
            entities_validator.LocationDataForUpdate(fields_to_update),
                std::make_pair(
                    false,
                    nullptr));

    const auto mask = entities_validator.GetMask();
    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Place))
    {
        location_to_update.place =
                entities_validator.GetStringAttribute(
                    EntitiesValidator::EntityAttributes::Place);
    }

    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Country))
    {
        location_to_update.country =
                entities_validator.GetStringAttribute(
                    EntitiesValidator::EntityAttributes::Country);
    }
    
    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::City))
    {
        location_to_update.city =
                entities_validator.GetStringAttribute(
                    EntitiesValidator::EntityAttributes::City);
    }
                
    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Distance))
    {
        location_to_update.distance =
                entities_validator.GetUint64Attribute(
                    EntitiesValidator::EntityAttributes::Distance);
    }
                
    return std::make_pair(
            true,
            std::make_unique<Location>(location_to_update));
}

std::pair<bool, std::unique_ptr<Visit>> RequestProcessor::FillVisitToUpdate(
        HttpParser& http_parser) const
{
    Visit visit_to_update;
    visit_to_update.id = http_parser.GetEntityId();

    const auto fields_to_update =
            http_parser.GetBodyContent();

    EntitiesValidator entities_validator;
    ENSURE_TRUE_OTHERWISE_RETURN(
            entities_validator.VisitDataForUpdate(fields_to_update),
                std::make_pair(
                    false,
                    nullptr));

    const auto mask = entities_validator.GetMask();
    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::LocationId))
    {
        visit_to_update.location_id =
                entities_validator.GetUint64Attribute(
                    EntitiesValidator::EntityAttributes::LocationId);
    }

    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::UserId))
    {
        visit_to_update.user_id =
                entities_validator.GetUint64Attribute(
                    EntitiesValidator::EntityAttributes::UserId);
    }

    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::VisitedAt))
    {
        visit_to_update.visited_at =
                entities_validator.GetInt64Attribute(
                    EntitiesValidator::EntityAttributes::VisitedAt);
    }

    if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Mark))
    {
        visit_to_update.mark =
                entities_validator.GetUint64Attribute(
                    EntitiesValidator::EntityAttributes::Mark);
    }
    
    return std::make_pair(
            true,
            std::make_unique<Visit>(visit_to_update));
}

template<typename T>
RequestProcessor::RequestProcessingResult RequestProcessor::ProcessAddEntityRequest(
        HttpParser& http_parser) const
{
    const auto data =
            http_parser.GetBodyContent();

    T entity;
    ENSURE_TRUE_OTHERWISE_RETURN(
            entity.Validate(data),
            std::make_pair(
                RequestProcessingStatus::BadRequest,
                std::make_unique<std::string>(
                    bad_data_response)));
                
    rapidjson::Document d;
    d.Parse(data);
    entity.Deserialize(d);

    ENSURE_TRUE_OTHERWISE_RETURN(
            data_storage_->AddEntity<T>(std::move(entity)) ==
                DataStorage::AddEntityStatus::EntitySuccessfullyAdded,
            std::make_pair(
                RequestProcessingStatus::BadRequest,
                std::make_unique<std::string>(
                    bad_data_response)));

    return std::make_pair(
            RequestProcessingStatus::Ok,
            std::make_unique<std::string>(empty_ok_response));
}