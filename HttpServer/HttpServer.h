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
#include "../DataStorage/Entities/EntitiesValidator.h"
//
#include "../Submodules/rapidjson/include/rapidjson/writer.h"
#include "../Submodules/rapidjson/include/rapidjson/stringbuffer.h"
//

namespace Network
{

enum class LoadedDataType
{
    Zipped = 0,
    Unzipped
};

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
        // Trace("175...");
        Trace("message_size = {}", message_size);
        Trace("message = {}", message);
        HttpParser http_parser;
        // ENSURE_TRUE_OTHERWISE_RETURN(
        //         http_parser.ParseHttpRequest(
        //             const_cast<char*>(message),
        //             message_size),
        //         std::make_pair(
        //             RequestProcessingStatus::BadRequest,
        //             nullptr));

        // TraceCharacters(
        //         const_cast<char*>(message),
        //         message_size);

        const auto parse_http_request_result =
                http_parser.ParseHttpRequest(
                    const_cast<char*>(message),
                    message_size);
        
        if (parse_http_request_result == HttpParser::ErrorType::ErrorTypeNotFound)
        {
            return std::make_pair(RequestProcessingStatus::NotFound, nullptr);
        }
        else if(parse_http_request_result == HttpParser::ErrorType::ErrorTypeBadRequest)
        {
            return std::make_pair(RequestProcessingStatus::BadRequest, nullptr);
        }

        DebugTrace("Before switch");
        std::unique_ptr<std::string> result;

        std::string result_str;

        DebugTrace("GetRequestType = {}", static_cast<int>(http_parser.GetRequestType()));
        DebugTrace("Body = {}\n\n\n", http_parser.GetBodyContent());

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

                result_str =
                        fmt::format(
                            get_entity_by_id_response,
                            result->size(),
                            *result);
            } break;

            case HttpParser::RequestType::UpdateUserById:
            {
                const auto id =
                        http_parser.GetEntityId();

                const auto data =
                        http_parser.GetBodyContent();

                User user;
                user.id_ = id;

                EntitiesValidator entities_validator;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        entities_validator.UserDataForUpdate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                const auto mask = entities_validator.GetMask();
                
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Email))
                {
                    user.email =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::Email);
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::FirstName))
                {
                    user.first_name =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::FirstName);
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::LastName))
                {
                    user.last_name =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::LastName);
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Gender))
                {
                    user.gender =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::Gender) == "m"
                                ? Gender::Male
                                : Gender::Female;
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::BirthDate))
                {
                    user.birth_date =
                            entities_validator.GetInt64Attribute(
                                EntitiesValidator::EntityAttributes::BirthDate);
                }
                            
                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->UpdateUser(user) ==
                            DataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::UpdateVisitById:
            {
                const auto id =
                        http_parser.GetEntityId();

                const auto data =
                        http_parser.GetBodyContent();

                Visit visit;
                visit.id_ = id;

                if (visit.id_ == 61127)
                {
                    Trace("visit.id_ == 29585");
                }

                EntitiesValidator entities_validator;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        entities_validator.VisitDataForUpdate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                const auto mask = entities_validator.GetMask();
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::LocationId))
                {
                    visit.location_id_ =
                            entities_validator.GetUint64Attribute(
                                EntitiesValidator::EntityAttributes::LocationId);
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::UserId))
                {
                    visit.user_id_ =
                            entities_validator.GetUint64Attribute(
                                EntitiesValidator::EntityAttributes::UserId);
                }
                
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::VisitedAt))
                {
                    visit.visited_at_ =
                            entities_validator.GetInt64Attribute(
                                EntitiesValidator::EntityAttributes::VisitedAt);
                }

                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Mark))
                {
                    visit.mark_ =
                            entities_validator.GetUint64Attribute(
                                EntitiesValidator::EntityAttributes::Mark);
                }
                            
                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->UpdateVisit(visit) ==
                            DataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::UpdateLocationById:
            {
                const auto id =
                        http_parser.GetEntityId();

                const auto data =
                        http_parser.GetBodyContent();

                Location location;
                location.id_ = id;

                EntitiesValidator entities_validator;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        entities_validator.LocationDataForUpdate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                const auto mask = entities_validator.GetMask();
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Place))
                {
                    location.place_ =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::Place);
                }

                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Country))
                {
                    location.country_ =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::Country);
                }

                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::City))
                {
                    location.city_ =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::City);
                }

                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Distance))
                {
                    location.distance_ =
                            entities_validator.GetUint64Attribute(
                                EntitiesValidator::EntityAttributes::Distance);
                }

                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->UpdateLocation(location) ==
                            DataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::AddUser:
            {
                const auto data =
                        http_parser.GetBodyContent();

                User user;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        user.Validate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));
                
                rapidjson::Document d;
                d.Parse(data);
                user.Deserialize(d);

                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->AddUser(std::move(user)) ==
                            DataStorage::AddEntityStatus::EntitySuccessfullyAdded,
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::AddVisit:
            {
                const auto data =
                        http_parser.GetBodyContent();

                Visit visit;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        visit.Validate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));
                
                rapidjson::Document d;
                d.Parse(data);
                visit.Deserialize(d);

                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->AddVisit(std::move(visit)) ==
                            DataStorage::AddEntityStatus::EntitySuccessfullyAdded,
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::AddLocation:
            {
                const auto data =
                        http_parser.GetBodyContent();

                Location location;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        location.Validate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                rapidjson::Document d;
                d.Parse(data);
                location.Deserialize(d);

                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->AddLocation(std::move(location)) ==
                            DataStorage::AddEntityStatus::EntitySuccessfullyAdded,
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                result_str = ok;
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
            const LoadedDataType loaded_data_type,
            const std::string& loaded_data_path,
            const size_t threads_count)
        : acceptor_(io_service_)
        , threads_(threads_count)
    {
        Trace("Data storage data loading...");

        data_storage_ = boost::make_shared<DataStorage>();
        
        // data_storage_->LoadZippedData("/tmp/data/data.zip");
        // data_storage_->LoadZippedData("/HighloadCup2017/data.zip");

        if (loaded_data_type == LoadedDataType::Zipped)
        {
            data_storage_->LoadZippedData(loaded_data_path);
        }
        else
        {
            data_storage_->LoadData(loaded_data_path);
        }

        // data_storage_->LoadZippedData("/home/egor/Repositories/highload_data_zip/data.zip");

        // data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/TRAIN/data/");

        // data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/FULL/data/");

        Trace("Data storage is ready...");
        //
        Trace("data_storage_->GetVisitsAmount() = {}", data_storage_->GetVisitsAmount());
        Trace("data_storage_->GetUsersAmount() = {}", data_storage_->GetUsersAmount());
        Trace("data_storage_->GetLocationsAmount() = {}", data_storage_->GetLocationsAmount());
        //

        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(locAddr, port);
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
        
        Trace("707...");

        StartAccept();
        
        std::generate(
              threads_.begin(),
              threads_.end(),
              boost::bind(
                  &boost::make_shared<boost::thread, boost::function<void ()> const &>,
                  boost::function<void ()>(
                      boost::bind(&boost::asio::io_service::run, &io_service_))));

        Trace("719...");
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


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

    enum class RequestProcessingStatus
    {
        Ok = 200, // 200
        NotFound = 404, // 404
        BadRequest = 400 // 400
    };

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

class session
  : public std::enable_shared_from_this<session>
  , public Traceable
{
    using RequestProcessingResult =
            std::pair<RequestProcessingStatus, std::unique_ptr<std::string>>;

/////////
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
/////////

    RequestProcessingResult ProcessRequest(
            const char* message,
            const size_t message_size)
    {
        // Trace("175...");
        HttpParser http_parser;
        // ENSURE_TRUE_OTHERWISE_RETURN(
        //         http_parser.ParseHttpRequest(
        //             const_cast<char*>(message),
        //             message_size),
        //         std::make_pair(
        //             RequestProcessingStatus::BadRequest,
        //             nullptr));

        // TraceCharacters(
        //         const_cast<char*>(message),
        //         message_size);

        const auto parse_http_request_result =
                http_parser.ParseHttpRequest(
                    const_cast<char*>(message),
                    message_size);
        
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

        DebugTrace("GetRequestType = {}", static_cast<int>(http_parser.GetRequestType()));
        DebugTrace("Body = {}\n\n\n", http_parser.GetBodyContent());

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

                result_str =
                        fmt::format(
                            get_entity_by_id_response,
                            result->size(),
                            *result);
            } break;

            case HttpParser::RequestType::UpdateUserById:
            {
                const auto id =
                        http_parser.GetEntityId();

                const auto data =
                        http_parser.GetBodyContent();

                User user;
                user.id_ = id;

                EntitiesValidator entities_validator;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        entities_validator.UserDataForUpdate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                const auto mask = entities_validator.GetMask();
                
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Email))
                {
                    user.email =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::Email);
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::FirstName))
                {
                    user.first_name =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::FirstName);
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::LastName))
                {
                    user.last_name =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::LastName);
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Gender))
                {
                    user.gender =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::Gender) == "m"
                                ? Gender::Male
                                : Gender::Female;
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::BirthDate))
                {
                    user.birth_date =
                            entities_validator.GetInt64Attribute(
                                EntitiesValidator::EntityAttributes::BirthDate);
                }
                            
                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->UpdateUser(user) ==
                            DataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::UpdateVisitById:
            {
                const auto id =
                        http_parser.GetEntityId();

                const auto data =
                        http_parser.GetBodyContent();

                Visit visit;
                visit.id_ = id;

                EntitiesValidator entities_validator;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        entities_validator.LocationDataForUpdate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                const auto mask = entities_validator.GetMask();
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::LocationId))
                {
                    visit.location_id_ =
                            entities_validator.GetUint64Attribute(
                                EntitiesValidator::EntityAttributes::LocationId);
                }
                            
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::UserId))
                {
                    visit.user_id_ =
                            entities_validator.GetUint64Attribute(
                                EntitiesValidator::EntityAttributes::UserId);
                }
                
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::VisitedAt))
                {
                    visit.visited_at_ =
                            entities_validator.GetInt64Attribute(
                                EntitiesValidator::EntityAttributes::VisitedAt);
                }

                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Mark))
                {
                    visit.mark_ =
                            entities_validator.GetUint64Attribute(
                                EntitiesValidator::EntityAttributes::Mark);
                }
                            
                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->UpdateVisit(visit) ==
                            DataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::UpdateLocationById:
            {
                const auto id =
                        http_parser.GetEntityId();

                const auto data =
                        http_parser.GetBodyContent();

                Location location;
                location.id_ = id;

                EntitiesValidator entities_validator;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        entities_validator.LocationDataForUpdate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                const auto mask = entities_validator.GetMask();
                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Place))
                {
                    location.place_ =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::Place);
                }

                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Country))
                {
                    location.country_ =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::Country);
                }

                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::City))
                {
                    location.city_ =
                            entities_validator.GetStringAttribute(
                                EntitiesValidator::EntityAttributes::City);
                }

                if (mask & static_cast<int>(EntitiesValidator::EntityAttributes::Distance))
                {
                    location.distance_ =
                            entities_validator.GetUint64Attribute(
                                EntitiesValidator::EntityAttributes::Distance);
                }

                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->UpdateLocation(location) ==
                            DataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated,
                        std::make_pair(
                            RequestProcessingStatus::NotFound,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::AddUser:
            {
                const auto data =
                        http_parser.GetBodyContent();

                User user;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        user.Validate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));
                
                rapidjson::Document d;
                d.Parse(data);
                user.Deserialize(d);

                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->AddUser(std::move(user)) ==
                            DataStorage::AddEntityStatus::EntitySuccessfullyAdded,
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::AddVisit:
            {
                const auto data =
                        http_parser.GetBodyContent();

                Visit visit;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        visit.Validate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));
                
                rapidjson::Document d;
                d.Parse(data);
                visit.Deserialize(d);

                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->AddVisit(std::move(visit)) ==
                            DataStorage::AddEntityStatus::EntitySuccessfullyAdded,
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                result_str = ok;
            } break;

            case HttpParser::RequestType::AddLocation:
            {
                const auto data =
                        http_parser.GetBodyContent();

                Location location;
                ENSURE_TRUE_OTHERWISE_RETURN(
                        location.Validate(data),
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                rapidjson::Document d;
                d.Parse(data);
                location.Deserialize(d);

                ENSURE_TRUE_OTHERWISE_RETURN(
                        data_storage_->AddLocation(std::move(location)) ==
                            DataStorage::AddEntityStatus::EntitySuccessfullyAdded,
                        std::make_pair(
                            RequestProcessingStatus::BadRequest,
                            nullptr));

                result_str = ok;
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

public:
  session(tcp::socket socket,
        boost::shared_ptr<DataStorage> data_storage)
    : socket_(std::move(socket))
    , data_storage_(data_storage)
  {
  }

  void start()
  {
    do_read();
  }

private:
  void do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {

                RequestProcessingResult process_request_result =
                        ProcessRequest(data_, length);

                std::unique_ptr<std::string> answer = nullptr; 

                if (process_request_result.first == RequestProcessingStatus::BadRequest)
                {
                    answer = std::make_unique<std::string>(bad_data_response);
                }
                else if (process_request_result.first == RequestProcessingStatus::NotFound)
                {
                    answer = std::make_unique<std::string>(not_found_post_response);
                }
                else if (process_request_result.first == RequestProcessingStatus::Ok)
                {
                    answer = std::move(process_request_result.second);
                }
                else
                {
                    answer = std::make_unique<std::string>("Default answer");
                }


            do_write(length);
          }
        });
  }

  void do_write(std::size_t length)
  {
    auto self(shared_from_this());

    std::string mes("Hello from server!!!!");
    boost::asio::async_write(socket_, boost::asio::buffer(mes, mes.size()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            do_read();
          }
        });
  }

  tcp::socket socket_;
  enum { max_length = 5024 };
  char data_[max_length];
  boost::shared_ptr<DataStorage> data_storage_;
};

class server : public Traceable
{
public:
  server(boost::asio::io_service& io_service, short port)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      socket_(io_service)
  {
      Trace("Data storage data loading...");

      data_storage_ = boost::make_shared<DataStorage>();
    //   data_storage_->LoadZippedData("/tmp/data/data.zip");
      data_storage_->LoadZippedData("/HighloadCup2017/data.zip");

      Trace("Data storage is ready...");

    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket_), data_storage_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
  boost::shared_ptr<DataStorage> data_storage_; 
};

// int main(int argc, char* argv[])
// {
//   try
//   {
//     if (argc != 2)
//     {
//       std::cerr << "Usage: async_tcp_echo_server <port>\n";
//       return 1;
//     }

//     boost::asio::io_service io_service;

//     server s(io_service, std::atoi(argv[1]));

//     io_service.run();
//   }
//   catch (std::exception& e)
//   {
//     std::cerr << "Exception: " << e.what() << "\n";
//   }

//   return 0;
// }






#endif // HTTP_SERVER_H_INCLUDED
