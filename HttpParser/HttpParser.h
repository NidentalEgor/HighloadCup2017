#ifndef HTTP_PARSER_H_INCLUDED
#define HTTP_PARSER_H_INCLUDED
#pragma once

#include <string>
#include <memory>

#include "../Submodules/http-parser/http_parser.h"
#include "../Submodules/libyuarel/yuarel.h"

#include "../DataStorage/Entities/DataTypes.h"
#include "../Utils/Traceable.h"

class HttpParser final
        : public Traceable
{
public:
    enum class HttpParserFlags
    {
        FromDateFlag   = 1,
        ToDateFlag     = 2,
        FromAgeFlag    = 4,
        ToAgeFlag      = 8,
        GenderFlag     = 16,
        CountryFlag    = 32,
        ToDistanceFlag = 64
    };

    enum class RequestType
    {
        None = 0,
        GetVisitById = 1,
        GetUserById = 2,
        GetLocationById = 3,
        GetVisitsByUserId = 4,
        GetAverageLocationMark = 5,
        UpdateVisitById = 6,
        UpdateUserById = 7,
        UpdateLocationById = 8,
        AddVisit = 9,
        AddUser = 10,
        AddLocation = 11
    };

    enum class ErrorType
    {
        ErrorTypeOk,
        ErrorTypeBadRequest,
        ErrorTypeNotFound
    };

    enum class HttpRequestType
    {
        HttpRequestTypeGet,
        HttpRequestTypePost
    };

    struct HttpRequest
    {
        yuarel_param url_params[500];
        int params_size = 0;
        const char* body = nullptr;
        size_t body_length = 0;
    };

    struct HttpData
    {
        HttpRequest request;
        unsigned int method;
        const char* url = nullptr;
        size_t url_length = 0;
    };

public:
    HttpParser();

    ErrorType ParseHttpRequest(
            /*const*/ char* request,
            const size_t readed);

    RequestType GetRequestType() const
    {
        return request_type_;
    }

    Id GetEntityId() const
    {
        return entity_id_;
    }

    int GetAdditionalInfoMask() const
    {
        return additional_info_mask_;
    }

    Timestamp GetFromDate() const
    {
        return from_date_;
    }

    Timestamp GetToDate() const
    {
        return to_date_;
    }

    Timestamp GetFromAge() const
    {
        return from_age_;
    }

    Timestamp GetToAge() const
    {
        return to_age_;
    }

    Gender GetGender() const
    {
        return gender_;
    }

    std::string GetCountry() const
    {
        return country;
    }

    Id GetToDistance() const
    {
        return to_distance_;
    }

    const char* GetBodyContent()
    {
        return entity_content_.c_str();
    }

    HttpRequestType GetHttpRequestType() const
    {
        return current_request_type_;
    }

private:
    void ParseHttpGetRequest(
            const char* request) const;

    static int OnUrl(
            http_parser* parser,
            const char* position,
            size_t length);
   
    static int OnBody(
            http_parser* parser,
            const char* position,
            size_t length);
    
    HttpParser::ErrorType Route(
            unsigned int method,    
            char** parts,
            const size_t parts_amount);
    
    HttpParser::ErrorType SplitQuery(
            char* query);

    std::pair<bool, long long> StringToNumber(
            const char* string);

private:
    std::unique_ptr<http_parser> parser_;
    http_parser_settings settings;
    RequestType request_type_;
    Id entity_id_;
    Timestamp from_date_;
    Timestamp to_date_;
    Timestamp from_age_;
    Timestamp to_age_;
    Gender gender_;
    std::string country;
    Id to_distance_;
    int additional_info_mask_;
    std::string entity_content_;
    HttpData http_data_;
    size_t MAX_PATH_SIZE = 10;
    HttpRequestType current_request_type_;
};

#endif // HTTP_PARSER_H_INCLUDED