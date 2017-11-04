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

namespace Network
{

namespace Private
{

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
            DataStorage::GetAverageLocationMarkQuery query)
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
            DataStorage::GetVisistsByUserIdQuery query)
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

    std::unique_ptr<std::string> ProcessRequest(
            const char* message,
            const size_t message_size)
    {
        HttpParser http_parser;
        ENSURE_TRUE_OTHERWISE_RETURN(
                http_parser.ParseHttpRequest(
                    const_cast<char*>(message),
                    message_size),
                nullptr);

        std::unique_ptr<std::string> result;
        switch (http_parser.GetRequestType())
        {
            case HttpParser::RequestType::GetUserById:
            {
                result =
                    data_storage_->GetUserById(http_parser.GetEntityId());
            } break;

            case HttpParser::RequestType::GetVisitById:
            {
                result =
                    data_storage_->GetVisitById(http_parser.GetEntityId());
            } break;

            case HttpParser::RequestType::GetLocationById:
            {
                result =
                    data_storage_->GetLocationById(http_parser.GetEntityId());
            } break;

            case HttpParser::RequestType::GetAverageLocationMark:
            {
                DataStorage::GetAverageLocationMarkQuery query(
                        http_parser.GetEntityId());
                
                const auto mask =
                        http_parser.GetAdditionalInfoMask();

                FillAverageLocationMarkQuery(
                        http_parser,
                        query);
                
                result =
                    data_storage_->GetAverageLocationMark(query);
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
            } break;

            default:
            {
                return nullptr;
            }
        }

        return result;
    }

    void HandleRead(
            boost::system::error_code const &error,
            std::size_t message_size)
    {
        if (error)
        {
            return;
        } 

        ///
        std::cout << "buffer_.data() = " << buffer_.data() << std::endl << std::endl;
        ///

        Trace("Request parsing begin...");
        const auto answer =
                ProcessRequest(buffer_.data(), message_size);
        Trace("Request parsing end...");
  
        if (!answer)
        {
            return;
        }

        std::cout << "Answer = " << *answer << std::endl;

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
            unsigned threadsCount)
        : acceptor_(io_service_)
        , threads_(threadsCount)
    {
        Trace("Data storage data loading...");

        data_storage_ = boost::make_shared<DataStorage>();
        data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/TRAIN/data/");

        Trace("Data storage is ready...");

        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(locAddr, port);
        boost::asio::ip::tcp::endpoint Endpoint = *resolver.resolve(query);
        acceptor_.open(Endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(Endpoint);
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
