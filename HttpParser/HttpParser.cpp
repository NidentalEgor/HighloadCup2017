#include <sys/uio.h>
#include <cstring>
#include <iostream>
#include <cstring>

#include "../Submodules/qs_parse/qs_parse.h"

#include "../Utils/Macroses.h"
#include "HttpParser.h"

const size_t HttpParser::MAX_PATH_SIZE = 10;

int HttpParser::OnUrl(
        http_parser* parser,
        const char* position,
        size_t length)
{
    HttpData* http_data =
            reinterpret_cast<HttpData*>(parser->data);

    http_data->url = position;
    http_data->url_length = length;

    return 0;
}
  
int HttpParser::OnBody(
        http_parser* parser,
        const char* position,
        size_t length)
{
    HttpData* http_data =
            reinterpret_cast<HttpData*>(parser->data);

    http_data->request.body = position;
    http_data->request.body_length = length;

    return 0;
}

HttpParser::HttpParser()
    : parser_(std::make_unique<http_parser>())
    , request_type_(RequestType::None)
    , entity_id_(std::numeric_limits<Id>::max())
    , from_date_(std::numeric_limits<Timestamp>::min())
    , to_date_(std::numeric_limits<Timestamp>::max())
    , from_age_(std::numeric_limits<Timestamp>::min())
    , to_age_(std::numeric_limits<Timestamp>::max())
    , gender_(Gender::Any)
    , country_()
    , to_distance_(std::numeric_limits<Distance>::max())
    , additional_info_mask_(0)
    , entity_content_()
    , http_data_()
    , current_request_type_(HttpRequestType::HttpRequestTypeAny)
{
    http_parser_init(parser_.get(), HTTP_BOTH);

    memset(&settings_, 0, sizeof(settings_));
    settings_.on_url = OnUrl;
    settings_.on_body = OnBody;
}

std::pair<bool, long long> HttpParser::StringToNumber(
        const char* string)
{
    size_t count = 0;
    while (string[count] != '\0')
    {
        ++count;
    }

    long long factor = 1;
    long long result = 0;
    for (int i = count - 1; i >= 0; --i)
    {
        if (string[i] < '0' || string[i] > '9')
        {
            return std::make_pair(false, 0);
        }

        int current_digit = string[i] - '0';
        result += factor * current_digit;
        factor *= 10;
    }

    return std::make_pair(true, result);
}

HttpParser::ErrorType HttpParser::Route(
        unsigned int method,    
        char** parts,
        const size_t parts_amount)
{
    DebugTrace("HttpParser::Route");

    request_type_ = RequestType::None;

    switch (method)
    {
        case HTTP_GET:
        {
            DebugTrace("case HTTP_GET");

            const auto id =
                    StringToNumber(parts[1]);

            ENSURE_TRUE_OTHERWISE_RETURN(
                    id.first,
                    ErrorType::ErrorTypeNotFound);

            entity_id_ = id.second;

            switch (parts[0][0])
            {
                case 'u':
                {
                    ENSURE_TRUE_OTHERWISE_RETURN(
                                strcmp(parts[0], "users") == 0,
                                ErrorType::ErrorTypeBadRequest);

                    if (parts_amount == 2)         // /users/<id>
                    {
                        request_type_ = RequestType::GetUserById;
                    }
                    else                           // /users/<id>/visits
                    {
                        ENSURE_TRUE_OTHERWISE_RETURN(
                                strcmp(parts[2], "visits") == 0,
                                ErrorType::ErrorTypeBadRequest);

                        request_type_ = RequestType::GetVisitsByUserId;
                    }

                } break;

                case 'l':
                {
                    ENSURE_TRUE_OTHERWISE_RETURN(
                                strcmp(parts[0], "locations") == 0,
                                ErrorType::ErrorTypeBadRequest);

                    if (parts_amount == 2)         // /locations/<id>
                    {
                        request_type_ = RequestType::GetLocationById;
                    }
                    else                           // /locations/<id>/avg
                    {
                        ENSURE_TRUE_OTHERWISE_RETURN(
                                strcmp(parts[2], "avg") == 0,
                                ErrorType::ErrorTypeBadRequest);

                        request_type_ = RequestType::GetAverageLocationMark;
                    }
                } break;

                case 'v':
                {
                    ENSURE_TRUE_OTHERWISE_RETURN(
                            strcmp(parts[0], "visits") == 0,
                            ErrorType::ErrorTypeBadRequest);

                    request_type_ = RequestType::GetVisitById;
                } break;

                default:
                {
                    return ErrorType::ErrorTypeBadRequest;
                }
            }
        } break;

        case HTTP_POST:
        {
            for (size_t i = 0; i < parts_amount; ++i)
            {
                DebugTrace(__FILENAME__, __LINE__, "part[{}] = {}", i, parts[i]);
            }

            switch (parts[0][0])
            {
                case 'u':
                {
                    DebugTrace(__FILENAME__, __LINE__, "case 'u':");

                    ENSURE_TRUE_OTHERWISE_RETURN(
                            strcmp(parts[0], "users") == 0,
                            ErrorType::ErrorTypeBadRequest);

                    if (strcmp(parts[1], "new") == 0)
                    {
                        DebugTrace(__FILENAME__, __LINE__, R"(if (strcmp(parts[1], "new") == 0))");
                        request_type_ = RequestType::AddUser;
                    }
                    else
                    {
                        DebugTrace(__FILENAME__, __LINE__, R"(NOT if (strcmp(parts[1], "new") == 0))");
                        const auto id =
                                StringToNumber(parts[1]);
                        ENSURE_TRUE_OTHERWISE_RETURN(
                                id.first,
                                ErrorType::ErrorTypeBadRequest);

                        entity_id_ = id.second;
                        request_type_ = RequestType::UpdateUserById;
                    }
                    
                } break;

                case 'l':
                {
                    DebugTrace(__FILENAME__, __LINE__, "case 'l':");

                    ENSURE_TRUE_OTHERWISE_RETURN(
                            strcmp(parts[0], "locations") == 0,
                            ErrorType::ErrorTypeBadRequest);
                    
                    if (strcmp(parts[1], "new") == 0)
                    {
                        DebugTrace(__FILENAME__, __LINE__, R"(if (strcmp(parts[1], "new") == 0))");
                        request_type_ = RequestType::AddLocation;
                    }
                    else
                    {
                        // DebugTrace(__FILENAME__, __LINE__, R"(NOT if (strcmp(parts[1], "new") == 0))");
                        // if (parts[1])
                        // {
                        //     DebugTrace(__FILENAME__, __LINE__, R"(parts[1] = {})", parts[1]);
                        // }
                        // else
                        // {
                        //     DebugTrace(__FILENAME__, __LINE__, R"(parts!!!11)");
                        // }
                        
                        const auto id =
                                StringToNumber(parts[1]);
                        ENSURE_TRUE_OTHERWISE_RETURN(
                                id.first,
                                ErrorType::ErrorTypeBadRequest);

                        entity_id_ = id.second;
                        request_type_ = RequestType::UpdateLocationById;
                    }
                    
                    
                } break;

                case 'v':
                {
                    ENSURE_TRUE_OTHERWISE_RETURN(
                            strcmp(parts[0], "visits") == 0,
                            ErrorType::ErrorTypeBadRequest);
                    
                    if (strcmp(parts[1], "new") == 0)
                    {
                        request_type_ = RequestType::AddVisit;
                    }
                    else
                    {
                        const auto id =
                                StringToNumber(parts[1]);
                        ENSURE_TRUE_OTHERWISE_RETURN(
                                id.first,
                                ErrorType::ErrorTypeBadRequest);
                        
                        entity_id_ = id.second;
                        request_type_ = RequestType::UpdateVisitById;
                    }
                } break;

                default:
                {
                    return ErrorType::ErrorTypeBadRequest;        
                }
            }
        } break;

        default:
        {
            return ErrorType::ErrorTypeBadRequest;
        }
    }

    return ErrorType::ErrorTypeOk;
}

HttpParser::ErrorType HttpParser::SplitQuery(
        char* query)
{
    static const size_t max_query_parameters_amount = 5;
    static struct yuarel_param query_parameters[max_query_parameters_amount];

    const auto query_parameters_amount =
            yuarel_parse_query(
                query,
                '&',
                query_parameters,
                max_query_parameters_amount);

    for (size_t i = 0; i < query_parameters_amount; ++i)
    {
        const auto key =
                query_parameters[i].key;
        const auto value =
                query_parameters[i].val;

        if (strcmp(key, "gender") == 0)
        {
            if (strcmp(value, "f") == 0)
            {
                gender_ = Gender::Female;
            }
            else if (strcmp(value, "m") == 0)
            {
                gender_ = Gender::Male;
            }
            else
            {
                return ErrorType::ErrorTypeBadRequest;
            }

            additional_info_mask_ |= static_cast<int>(HttpParserFlags::GenderFlag);
        }
        else if (strcmp(key, "toDistance") == 0)
        {
            const auto to_distance =
                    StringToNumber(value);
            
            ENSURE_TRUE_OTHERWISE_RETURN(
                    to_distance.first,
                    ErrorType::ErrorTypeBadRequest);
            
            to_distance_ = to_distance.second;
            additional_info_mask_ |= static_cast<int>(HttpParserFlags::ToDistanceFlag);
        }
        else if (strcmp(key, "fromDate") == 0)
        {
            const auto from_date =
                    StringToNumber(value);
            
            ENSURE_TRUE_OTHERWISE_RETURN(
                    from_date.first,
                    ErrorType::ErrorTypeBadRequest);
            
            from_date_ = from_date.second;
            additional_info_mask_ |= static_cast<int>(HttpParserFlags::FromDateFlag);
        }
        else if (strcmp(key, "toDate") == 0)
        {
            const auto to_date =
                    StringToNumber(value);
            
            ENSURE_TRUE_OTHERWISE_RETURN(
                    to_date.first,
                    ErrorType::ErrorTypeBadRequest);
            
            to_date_ = to_date.second;
            additional_info_mask_ |= static_cast<int>(HttpParserFlags::ToDateFlag);
        }
        else if (strcmp(key, "fromAge") == 0)
        {
            const auto from_age =
                    StringToNumber(value);
            
            ENSURE_TRUE_OTHERWISE_RETURN(
                    from_age.first,
                    ErrorType::ErrorTypeBadRequest);
            
            from_age_ = from_age.second;
            additional_info_mask_ |= static_cast<int>(HttpParserFlags::FromAgeFlag);
        }
        else if (strcmp(key, "toAge") == 0)
        {
            const auto to_age =
                    StringToNumber(value);
            
            ENSURE_TRUE_OTHERWISE_RETURN(
                    to_age.first,
                    ErrorType::ErrorTypeBadRequest);
            
            to_age_ = to_age.second;
            additional_info_mask_ |= static_cast<int>(HttpParserFlags::ToAgeFlag);
        }
        else if (strcmp(key, "country") == 0)
        {
            qs_decode(value);

            country_ = value;
            additional_info_mask_ |= static_cast<int>(HttpParserFlags::CountryFlag);
        }
    }

    return ErrorType::ErrorTypeOk;
}

HttpParser::ErrorType HttpParser::ParseHttpRequest(
        /*const*/ char* request,
        const size_t readed)
{
    additional_info_mask_ = 0;

    char* request_local = request;

    if (request_local[0] == 'G')
    {
        char* url_start = request_local + 4;
        http_data_.url = url_start;
        char* it = url_start;
        int url_len = 0;

        while (*it++ != ' ')
        {
          ++url_len;
        }

        http_data_.url_length = url_len;
        http_data_.method = HTTP_GET;

        DebugTrace(__FILENAME__, __LINE__, "Method = {}", parser_->method);
    }
    else
    {
        http_parser_init(
                parser_.get(),
                HTTP_REQUEST);
        parser_->data = &http_data_;
        const int nparsed = http_parser_execute(
                parser_.get(),
                &settings_,
                request_local,
                readed);

        if (nparsed != readed)
        {
          DebugTrace("nparsed = {}", nparsed);
          // close(sock);
          // continue;
        }

        entity_content_ =
                std::string(
                    http_data_.request.body,
                    http_data_.request.body_length);

        DebugTrace(
                "parser_->http_errno = {}",
                http_errno_description(HTTP_PARSER_ERRNO(parser_.get())));

        http_data_.method = parser_->method;

    }

    current_request_type_ =
            http_data_.method == HTTP_GET
            ? HttpRequestType::HttpRequestTypeGet
            : HttpRequestType::HttpRequestTypePost; 

    DebugTrace("Url start...");

    static char max_path[120];

    // Not for debug trace!!!
    
    for (size_t i = 0; i < http_data_.url_length; ++i)
    {
        max_path[i] = http_data_.url[i];
    }

    // Not for debug trace!!!

    max_path[http_data_.url_length] = '\0';
    DebugTrace("Url end...");

    struct yuarel url;
    if (yuarel_parse(&url, const_cast<char*>(max_path)) == -1)
    {
        DebugTrace("Error!!!");
    }
    else
    {
// temp
#ifdef DEBUG_LOG
        printf("Struct values:\n");
        printf("\tscheme:\t\t%s\n", url.scheme);
        printf("\thost:\t\t%s\n", url.host);
        printf("\tport:\t\t%d\n", url.port);
        printf("\tpath:\t\t%s\n", url.path);
        printf("\tquery:\t\t%s\n", url.query);
        printf("\tfragment:\t%s\n", url.fragment);
#endif
// temp
#ifdef DEBUG_LOG
        printf("before yuarel_split_path\n"); 
#endif

        char* parts[MAX_PATH_SIZE];
        auto path_parts_amount = yuarel_split_path(
                url.path,
                parts,
                MAX_PATH_SIZE);

#ifdef DEBUG_LOG
        printf("after yuarel_split_path\n");
        printf("path_parts_amount = %d\n", path_parts_amount);
#endif

        if (url.query != nullptr)
        {
#ifdef DEBUG_LOG
        printf("before SplitQuery\n");
#endif
            const auto query_split_result =
                    SplitQuery(url.query);
#ifdef DEBUG_LOG
        printf("after SplitQuery\n");
        printf("query_split_result = %d\n", static_cast<int>(query_split_result));
#endif
            ENSURE_TRUE_OTHERWISE_RETURN(
                    query_split_result == HttpParser::ErrorType::ErrorTypeOk,
                    query_split_result);
        }
        
        for (size_t i = 0; i < path_parts_amount; ++i)
        {
            DebugTrace("part[{}] = {}", i, parts[i]);
        }
      
        if (http_data_.request.body != nullptr)
        {
            DebugTrace("Request body is not empty");
        }
        else
        {
            DebugTrace("Request body is empty");
        }

        const auto route_result =
                Route(http_data_.method, parts, path_parts_amount);
        
        ENSURE_TRUE_OTHERWISE_RETURN(
                route_result == ErrorType::ErrorTypeOk,
                route_result);

        DebugTrace("route_result = {}", static_cast<int>(route_result));
    }

    return HttpParser::ErrorType::ErrorTypeOk;
}
