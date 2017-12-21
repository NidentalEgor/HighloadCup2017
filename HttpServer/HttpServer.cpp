#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/epoll.h>

#include <thread>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include "HttpServer.h"

const size_t MAX_EVENTS = 1000;
const size_t MAX_MESSAGE_SIZE = 2048;

int SetNonblock(const int fd)
{
    int flags =0;

#ifdef O_NONBLOCK
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
    {
        flags = 0;
    }

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

void DoJob(
        const int epoll_fd,
        const std::shared_ptr<RequestProcessor>& request_processor)
{
    struct epoll_event events[MAX_EVENTS];

    while (true)
    {
        const int amount =
                epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int index = 0; index < amount; ++index)
        {
            char buffer[MAX_MESSAGE_SIZE];
            const int message_size =
                    recv(
                        events[index].data.fd,
                        buffer,
                        1024,
                        MSG_NOSIGNAL);

            // if ((message_size == 0) && (errno != EAGAIN))
            // {
            //     shutdown(
            //             events[index].data.fd,
            //             SHUT_RDWR);
            //     close(events[index].data.fd);
            // }
            /*else*/
            if (message_size > 0)
            {
                const auto result =
                        request_processor->ProcessRequest(
                            buffer,
                            message_size);
                
                if (result.first ==
                    RequestProcessor::RequestProcessingStatus::HttpParserUnknownRequestType)
                {
                    break;
                }

                //////
                std::cout.write(
                        buffer,
                        message_size);
                std::cout << std::endl;
                //////

                #ifdef DEBUG_LOG
                std::cout << "result.first = " << static_cast<int>(result.first) << std::endl;
                #endif

                #ifdef DEBUG_LOG
                std::cout.write(
                        result.second->c_str(),
                        result.second->size() + 1);
                std::cout << std::endl;
                #endif

                send(
                    events[index].data.fd,
                    result.second->c_str(),
                    result.second->size() + 1,
                    MSG_NOSIGNAL);

                // shutdown(
                //     events[index].data.fd,
                //     SHUT_RDWR);

                // close(events[index].data.fd);
            }

            shutdown(
                events[index].data.fd,
                SHUT_RDWR);

            close(events[index].data.fd);
        }
    }
}

HttpServer::HttpServer(
        const std::string& ip_address,
        const short port,
        const LoadedDataType loaded_data_type,
        const std::string& loaded_data_path,
        const size_t threads_count)
    : ip_address_(ip_address)
    , port_(port)
{
    Trace("Data storage data loading...");

    std::shared_ptr<DataStorage> data_storage = std::make_shared<DataStorage>();

    if (loaded_data_type == LoadedDataType::Zipped)
    {
        data_storage->LoadZippedData(loaded_data_path);
    }
    else
    {
        data_storage->LoadData(loaded_data_path);
    }

    Trace("Data storage is ready...");
    Trace("data_storage->GetVisitsAmount() = {}", data_storage->GetVisitsAmount());
    Trace("data_storage->GetUsersAmount() = {}", data_storage->GetUsersAmount());
    Trace("data_storage->GetLocationsAmount() = {}", data_storage->GetLocationsAmount());

    request_processor_ = std::make_shared<RequestProcessor>(data_storage);
}

void HttpServer::Run()
{
    const int master_socket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port_);
    sock_addr.sin_addr.s_addr = inet_addr(ip_address_.c_str());
    
    bind(master_socket,
        reinterpret_cast<struct sockaddr*>(&sock_addr),
        sizeof(sock_addr));

    signal(SIGPIPE, SIG_IGN);

    auto listen_ret = listen(master_socket, SOMAXCONN);
    if (listen_ret == -1)
    {
        // DebugTrace("if (listen_ret == -1)");
    }

    // auto set_nonblock_ret = SetNonblock(master_socket);

    // if (set_nonblock_ret == -1)
    // {
    //     // DebugTrace("if (set_nonblock_ret == -1)");
    // }

    int epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    std::vector<std::thread> threads;
    for (size_t i = 0; i < 3; ++i)
    {
        threads.emplace_back(DoJob, epollfd, request_processor_);
    }

    int new_buf_size = 1024 * 512;
    int i = 1;
    while (true)
    {
        int sock = accept(master_socket, NULL, NULL);
        if (sock == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        struct epoll_event event;
        event.data.fd = sock;
        event.events = EPOLLIN;

        // auto set_nonblock_ret = SetNonblock(master_socket);

        // if (set_nonblock_ret == -1)
        // {
        //     // DebugTrace("if (set_nonblock_ret == -1)");
        //     std::cout << "170" << std::endl;
        // }

        // epoll_ctl(
        //         epollfd,
        //         EPOLL_CTL_ADD,
        //         sock,
        //         &event);

        setsockopt(sock, SOL_SOCKET, SO_SNDBUF,
                   (void *)&new_buf_size, sizeof(new_buf_size));
        setsockopt(sock, SOL_SOCKET, SO_RCVBUF,
                   (void *)&new_buf_size, sizeof(new_buf_size));
        setsockopt(sock, SOL_SOCKET, SO_DONTROUTE, (void *)&i, sizeof(i));
        // setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&i, sizeof(i));
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        ev.data.fd = sock;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev) == -1)
        {
            perror("epoll_ctl: conn_sock");
            exit(EXIT_FAILURE);
        }
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
}