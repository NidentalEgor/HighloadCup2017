#include <sys/uio.h>
#include <cstring>
#include <iostream>

#include "yuarel.h"

#include "../Utils/Macroses.h"

#include "HttpParser.h"

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

HttpData http_data;

int HttpParser::OnUrl(
        http_parser* parser,
        const char* position,
        size_t length)
{
    std::cout << "OnUrl" << std::endl;
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
    std::cout << "OnBody" << std::endl;
    HttpData* http_data =
            reinterpret_cast<HttpData*>(parser->data);
    http_data->request.body = position;
    http_data->request.body_length = length;
    return 0;
}

HttpParser::HttpParser()
    : request_type_(RequestType::None)
{
    parser_ = std::make_unique<http_parser>();
    http_parser_init(parser_.get(), HTTP_BOTH);

    memset(&settings, 0, sizeof(settings));
    settings.on_url = OnUrl;
    settings.on_body = OnBody;
}

std::pair<bool, long long> StringToNumber(
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

bool HttpParser::Route(
        char** parts,
        unsigned int method,
        const size_t parts_amount)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            (parts_amount > 3 || parts_amount < 2),
            false);

    switch (method)
    {
        case HTTP_GET:
        {
            const auto id =
                    StringToNumber(parts[1]);

            ENSURE_TRUE_OTHERWISE_RETURN(id.first, false);

            entity_id_ = id.second;

            switch (parts[0][0])
            {
                case 'u':
                {
                    if (parts_amount == 2)         // /users/<id>
                    {
                        request_type_ = RequestType::GetUserById;
                    }
                    else                           // /users/<id>/visits
                    {
                        request_type_ = RequestType::GetVisitsByUserId;
                    }

                } break;

                case 'l':
                {
                    if (parts_amount == 2)         // /locations/<id>
                    {
                        request_type_ = RequestType::GetUserById;
                    }
                    else                           // /locations/<id>/avg
                    {
                        request_type_ = RequestType::GetAverageLocationMark;
                    }
                } break;

                case 'v':
                {
                    request_type_ = RequestType::GetVisitById;
                } break;

                default:
                {
                    return false;
                }
            }
        } break;

        case HTTP_POST:
        {
            // TODO
        } break;

        default:
        {
            return false;
        }
    }
}

bool HttpParser::ParseHttpRequest(
        /*const*/ char* request,
        const size_t readed)
{
    char* request_local = request;

    // HttpData http_data;
    if (request_local[0] == 'G')
    {
        char* url_start = request_local + 4;
        http_data.url = url_start;
        char* it = url_start;
        int url_len = 0;

        while (*it++ != ' ')
        {
          ++url_len;
        }

        http_data.url_length = url_len;
        http_data.method = HTTP_GET;
    }
    else
    {
        http_parser_init(
                parser_.get(),
                HTTP_REQUEST);
        parser_->data = &http_data;
        const int nparsed = http_parser_execute(
                parser_.get(),
                &settings,
                request_local,
                readed);

        if (nparsed != readed)
        {
          std::cout << "nparsed = " << nparsed << std::endl;

          // close(sock);
          // continue;
        }

        std::cout << "parser_->http_errno = " <<
                http_errno_description(HTTP_PARSER_ERRNO(parser_.get())) << std::endl;

        http_data.method = parser_->method;
    }

    std::cout << "Url start..." << std::endl;

    static char max_path[120];

    for (size_t i = 0; i < http_data.url_length; ++i)
    {
        max_path[i] = http_data.url[i];
        std::cout << *(http_data.url + i);
    }
    max_path[http_data.url_length] = '\0';
    std::cout << std::endl << "Url end..." << std::endl;

    struct yuarel url;
    if (yuarel_parse(&url, const_cast<char*>(max_path)) == -1)
    {
        std::cout << "Error!!!" << std::endl;
    }
    else
    {
        printf("Struct values:\n");
        printf("\tscheme:\t\t%s\n", url.scheme);
        printf("\thost:\t\t%s\n", url.host);
        printf("\tport:\t\t%d\n", url.port);
        printf("\tpath:\t\t%s\n", url.path);
        printf("\tquery:\t\t%s\n", url.query);
        printf("\tfragment:\t%s\n", url.fragment);

        char* parts[MAX_PATH_SIZE];
        auto ret = yuarel_split_path(
                url.path,
                parts,
                MAX_PATH_SIZE);
        
        std::cout << "ret = " << ret << std::endl;
        
        for (size_t i = 0; i < ret; ++i)
        {
            std::cout << "part[" << i <<"] = " << parts[i] << std::endl;
        }
      
        const auto res = StringToNumber(parts[1]);

        std::cout << "Body = " << http_data.request.body << std::endl;

        const bool route_result =
                Route(parts, http_data.method, ret);
    }

    return true;
}

void HttpParser::ParseHttpPostRequest(
        const char* request) const
{
    // /<entity>/<id>
    // /<entity>/new


}
