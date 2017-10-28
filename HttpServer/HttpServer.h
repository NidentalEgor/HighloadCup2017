#ifndef HTTP_SERVER_H_INCLUDED
#define HTTP_SERVER_H_INCLUDED
#pragma once

// class HttpServer
// {
// public:
//     HttpServer();
//     void Run();
// private:

// };

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>

#include "DataStorage.h"

namespace Network
{

namespace Private
{

class Connection
    : private boost::noncopyable
    , public boost::enable_shared_from_this<Connection>
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

    void HandleRead(boost::system::error_code const &error, std::size_t bytes)
    {
        if (error)
        {
            return;
        } 
        
        // std::cout << "buffer_.data() = " << buffer_.data() << std::endl;  
        // const uint32_t id = buffer_.data()[0] - '0'; 
        // const auto answer = data_storage_->GetUserById(id);
        ///
        auto answer = std::make_unique<std::string>("{}");
        ///
        if (answer)
        {
            // std::cout << "Answer = " << *answer << std::endl;

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
{
public:
    EchoServer(
            const std::string& locAddr,
            const std::string& port,
            unsigned threadsCount)
        : acceptor_(io_service_)
        , threads_(threadsCount)
    {
        std::cout << "Data storage data loading..." << std::endl;

        data_storage_ = boost::make_shared<DataStorage>();
        data_storage_->LoadData("/home/egor/Repositories/hlcupdocs/data/TRAIN/data/");

        std::cout << "Data storage is ready..." << std::endl;

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
