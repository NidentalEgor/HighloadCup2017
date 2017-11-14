#ifndef HTTP_SERVER_H_INCLUDED
#define HTTP_SERVER_H_INCLUDED
#pragma once

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>

#include "../HttpParser/HttpParser.h"
#include "../Utils/Macroses.h"
#include "../Utils/Traceable.h"
#include "../DataStorage/DataStorage.h"

//
#include "../Submodules/rapidjson/include/rapidjson/writer.h"
#include "../Submodules/rapidjson/include/rapidjson/stringbuffer.h"
//

namespace Network
{

namespace Private
{
    enum class RequestProcessingStatus
    {
        Ok = 200, // 200
        NotFound = 404, // 404
        BadRequest = 400 // 400
    };

    using RequestProcessingResult =
            std::pair<RequestProcessingStatus, std::unique_ptr<std::string>>;

    static const std::string not_found_post_response(
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
    
    static const std::string get_entity_by_id_response(
            "HTTP/1.1 200 OK\r\n"
            "S: b\r\n"
            "C: k\r\n"
            "B: a\r\n"
            "Content-Length: {}\r\n"
            "\r\n"
            "{}");

    // Not used yet.
    static const std::string ok(
            "HTTP/1.1 200 OK\r\n"
            "S: b\r\n"
            "C: k\r\n"
            "B: a\r\n"
            "Content-Length: 2\r\n"
            "\r\n"
            "{}");

class Connection
    : private boost::noncopyable
    , public boost::enable_shared_from_this<Connection>
    , public Traceable
{
public:
    Connection(
            boost::asio::io_service &ioService,
            boost::shared_ptr<DataStorage> data_storage)
        : strand_(ioService)
        , socket_(ioService)
        , data_storage_(data_storage)
    {
    }

    ~Connection()
    {
    }

    boost::asio::ip::tcp::socket& GetSocket()
    {
        return socket_;
    }

    void Start()
    {
        socket_.async_read_some(
                boost::asio::buffer(buffer_),
                strand_.wrap(
                    boost::bind(
                        &Connection::HandleRead,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
    }

    void FillAverageLocationMarkQuery(
            const HttpParser& http_parser,
            DataStorage::GetAverageLocationMarkQuery& query)
    {
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
    }

    void FillVisistsByUserIdQuery(
            const HttpParser& http_parser,
            DataStorage::GetVisistsByUserIdQuery& query)
    {
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
    }

    RequestProcessingResult ProcessRequest(
            const char* message,
            const size_t message_size)
    {
        HttpParser http_parser;
        // ENSURE_TRUE_OTHERWISE_RETURN(
        //         http_parser.ParseHttpRequest(
        //             const_cast<char*>(message),
        //             message_size),
        //         std::make_pair(
        //             RequestProcessingStatus::BadRequest,
        //             nullptr));

        // Trace("179");
        // Trace("179");
        // TraceCharacters(
        //         const_cast<char*>(message),
        //         message_size);

        const auto parse_http_request_result =
                http_parser.ParseHttpRequest(
                    const_cast<char*>(message),
                    message_size);
        
        // Trace("parse_http_request_result = {}", static_cast<int>(parse_http_request_result));

        if (parse_http_request_result == HttpParser::ErrorType::ErrorTypeNotFound)
        {
            return std::make_pair(RequestProcessingStatus::NotFound, nullptr);
        }
        else
        if(parse_http_request_result == HttpParser::ErrorType::ErrorTypeBadRequest)
        {
            return std::make_pair(RequestProcessingStatus::BadRequest, nullptr);
        }

        DebugTrace("Before switch");
        std::unique_ptr<std::string> result;

        std::string result_str;

        // Trace("GetRequestType = {}", static_cast<int>(http_parser.GetRequestType()));

        switch (http_parser.GetRequestType())
        {
            case HttpParser::RequestType::GetUserById:
            {
                result =
                    data_storage_->GetUserById(http_parser.GetEntityId());

                ENSURE_TRUE_OTHERWISE_RETURN(
                        result,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));

                result_str =
                        fmt::format(
                            get_entity_by_id_response,
                            result->size(),
                            *result);
            } break;

            case HttpParser::RequestType::GetVisitById:
            {
                result =
                    data_storage_->GetVisitById(http_parser.GetEntityId());

                ENSURE_TRUE_OTHERWISE_RETURN(
                        result,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));
                
                result_str =
                        fmt::format(
                            get_entity_by_id_response,
                            result->size(),
                            *result);
            } break;

            case HttpParser::RequestType::GetLocationById:
            {
                result =
                    data_storage_->GetLocationById(http_parser.GetEntityId());

                ENSURE_TRUE_OTHERWISE_RETURN(
                        result,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));

                result_str =
                        fmt::format(
                            get_entity_by_id_response,
                            result->size(),
                            *result);
            } break;

            case HttpParser::RequestType::GetAverageLocationMark:
            {
                DataStorage::GetAverageLocationMarkQuery query(
                        http_parser.GetEntityId());

                FillAverageLocationMarkQuery(
                        http_parser,
                        query);

                result =
                    data_storage_->GetAverageLocationMark(query);
                
                ENSURE_TRUE_OTHERWISE_RETURN(
                        result,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));

                result_str =
                        fmt::format(
                            get_entity_by_id_response,
                            result->size(),
                            *result);
            } break;

            case HttpParser::RequestType::GetVisitsByUserId:
            {
                DataStorage::GetVisistsByUserIdQuery query(
                        http_parser.GetEntityId());

                FillVisistsByUserIdQuery(
                        http_parser,
                        query);

                result =
                    data_storage_->GetVisistsByUserId(query);

                ENSURE_TRUE_OTHERWISE_RETURN(
                        result,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));
                
                // Trace("result = {}", *result);

                result_str =
                        fmt::format(
                            get_entity_by_id_response,
                            result->size(),
                            *result);
                
                // Trace("316 316 316 316 316 316 316 316 316 316 result_str = {}", result_str);
            } break;

            case HttpParser::RequestType::UpdateUserById:
            {

            } break;

            case HttpParser::RequestType::UpdateVisitById:
            {
                
            } break;

            case HttpParser::RequestType::UpdateLocationById:
            {

            } break;

            case HttpParser::RequestType::AddUser:
            {
                // const auto id =
                //         http_parser.GetEntityId();
                
                // const auto data =
                //         http_parser.GetBodyContent();
                // Trace(__FILENAME__, __LINE__, "body = {}", data);

                // rapidjson::Document d;
                // d.Parse(data);
                // // User user(d);
                // // Value& s = d[""];

                // rapidjson::StringBuffer buffer;
                // rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                // d.Accept(writer);

                // // Output {"project":"rapidjson","stars":11}
                // Trace("SEE SEE SEE SEE SEE SEE SEE SEE SEE SEE ");
                // std::cout << buffer.GetString() << std::endl;

            } break;

            case HttpParser::RequestType::AddVisit:
            {
                const auto id =
                        http_parser.GetEntityId();
                
                const auto data =
                        http_parser.GetBodyContent();

                Visit visit;
                const auto validate_result = visit.Validate(data);
                
                Trace(__FILENAME__, __LINE__, "data = {}", data);
                Trace(__FILENAME__, __LINE__, "validate_result = {}\n\n", validate_result);
            } break;

            case HttpParser::RequestType::AddLocation:
            {
                const auto id =
                        http_parser.GetEntityId();
                
                const auto data =
                        http_parser.GetBodyContent();
                // Trace(__FILENAME__, __LINE__, "body = {}", data);


            } break;

            default:
            {
                // return nullptr;
            }
        }

        DebugTrace("Before common return");
        return std::make_pair(
                RequestProcessingStatus::Ok,
                std::make_unique<std::string>(result_str));
    }

    void HandleRead(
            boost::system::error_code const &error,
            std::size_t message_size)
    {
        if (error)
        {
            return;
        } 

        DebugTrace("\n");
        DebugTraceCharacters("buffer_.data() = {}", message_size);

        DebugTrace("Request parsing begin.....");
        RequestProcessingResult process_request_result =
                ProcessRequest(buffer_.data(), message_size);
        DebugTrace("Request parsing end...");
  
        std::unique_ptr<std::string> answer = nullptr; 

        DebugTrace("Before process_request_result.first 2 ");
        if (process_request_result.first == RequestProcessingStatus::BadRequest)
        {
            DebugTrace("RequestProcessingStatus:BadRequest");
            answer = std::make_unique<std::string>(bad_data_response);
        }
        else if (process_request_result.first == RequestProcessingStatus::NotFound)
        {
            DebugTrace("RequestProcessingStatus:NotFound");
            answer = std::make_unique<std::string>(not_found_post_response);
        }
        else if (process_request_result.first == RequestProcessingStatus::Ok)
        {
            DebugTrace("RequestProcessingStatus::Ok");
            answer = std::move(process_request_result.second);
        }
        else
        {
            answer = std::make_unique<std::string>("Default answer");
        }

        DebugTrace("Answer = {}", *answer);

        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(
                boost::asio::const_buffer(
                    answer->c_str(),
                    answer->size()));

        boost::asio::async_write(
            socket_,
            buffers,
            strand_.wrap(
                boost::bind(
                    &Connection::HandleWrite,
                    shared_from_this(),
                    boost::asio::placeholders::error)));
    }

    void HandleWrite(
            const boost::system::error_code& error)
    {
        if (error)
        {
            // Trace("Error Error Error Error Error Error Error Error Error Error Error Error Error ");
            return;
        }

        boost::system::error_code error_code;
        socket_.shutdown(
                boost::asio::ip::tcp::socket::shutdown_both,
                error_code);
        
    }
  
private:
  boost::array<char, 4096> buffer_;
  boost::asio::io_service::strand strand_;
  boost::asio::ip::tcp::socket socket_;
  boost::shared_ptr<DataStorage> data_storage_;
};
    
} // namespace Private



class EchoServer
    : private boost::noncopyable
    , public Traceable
{
public:
    EchoServer(
            const std::string& locAddr,
            const std::string& port,
            unsigned threadsCount)
        : acceptor_(io_service_)
        , threads_(threadsCount)
    {
        Trace("Data storage data loading...");

        data_storage_ = boost::make_shared<DataStorage>();
        data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/TRAIN/data/");
        // data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/FULL/data/");

        Trace("Data storage is ready...");

        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(locAddr, port);
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
        
        StartAccept();
        
        std::generate(
              threads_.begin(),
              threads_.end(),
              boost::bind(
                  &boost::make_shared<boost::thread, boost::function<void ()> const &>,
                  boost::function<void ()>(
                      boost::bind(&boost::asio::io_service::run, &io_service_))));
    }
    ~EchoServer()
    {
        std::for_each(
            threads_.begin(),
            threads_.end(),
            boost::bind(&boost::asio::io_service::stop, &io_service_));

        std::for_each(
            threads_.begin(),
            threads_.end(),
            boost::bind(&boost::thread::join, _1));
    }

private:
    void StartAccept()
    {
        new_connection_ =
                boost::make_shared<Private::Connection, boost::asio::io_service &>(
                    io_service_, data_storage_);
        acceptor_.async_accept(
            new_connection_->GetSocket(),
            boost::bind(
                &EchoServer::HandleAccept,
                this,
                boost::asio::placeholders::error));
    }

    void HandleAccept(boost::system::error_code const &error)
    {
        if (!error)
        {
            new_connection_->Start();
        }
        StartAccept();
    }

private:
    using ThreadPtr = boost::shared_ptr<boost::thread>;
    using ThreadPool = std::vector<ThreadPtr>;
    using ConnectionPtr = boost::shared_ptr<Private::Connection>;

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    ConnectionPtr new_connection_;
    ThreadPool threads_;
    boost::shared_ptr<DataStorage> data_storage_;
  };

} // namespace Network

#endif // HTTP_SERVER_H_INCLUDED
