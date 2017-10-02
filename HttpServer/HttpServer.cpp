#include <thread>
#include <vector>

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>

#include <fstream>
#include <string>
#include <sstream>

#include "HttpServer.h"

const size_t MAX_EVENTS = 32;

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

void DoJob()
{
    // std::stringstream file_name;
    // file_name << "Thread" << std::this_thread::get_id();
    // std::ofstream out(file_name.str().c_str());
    // out << "My id = " << std::this_thread::get_id() << std::endl;
    // out.close();

    std::cout << "Hello from thread " <<
        std::this_thread::get_id() << std::endl;

    const int master_socket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(12345);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(master_socket,
        (struct sockaddr*)(&sock_addr),
        sizeof(sock_addr));

    auto set_nonblock_ret = SetNonblock(master_socket);
    if (set_nonblock_ret == -1)
    {
        std::cout << "if (set_nonblock_ret == -1)" << std::endl;
    }

    auto listen_ret = listen(master_socket, SOMAXCONN);
    if (listen_ret == -1)
    {
        std::cout << "if (listen_ret == -1)" << std::endl;
    }

    const int EPoll = epoll_create1(0);

    struct epoll_event event;
    event.data.fd = master_socket;
    event.events = EPOLLIN;

    epoll_ctl(
            EPoll,
            EPOLL_CTL_ADD,
            master_socket,
            &event);

    struct epoll_event events[MAX_EVENTS];

    size_t index = 0;

    while (true && index < 3)
    {
        const int amount =
                epoll_wait(EPoll, events, MAX_EVENTS, -1);

        for (int index = 0; index < amount; ++index)
        {
            if (events[index].data.fd == master_socket)
            {
                const int slave_socket =
                        accept(master_socket, 0, 0);
                SetNonblock(slave_socket);
                struct epoll_event new_event;
                new_event.data.fd = slave_socket;
                new_event.events = EPOLLIN;

                epoll_ctl(
                        EPoll,
                        EPOLL_CTL_ADD,
                        slave_socket,
                        &new_event);
            }
            else
            {
                char buffer[1024];
                const int message_size =
                        recv(
                            events[index].data.fd,
                            buffer,
                            1024,
                            MSG_NOSIGNAL);

                if ((message_size == 0) && (errno != EAGAIN))
                {
                    shutdown(
                            events[index].data.fd,
                            SHUT_RDWR);
                    close(events[index].data.fd);
                }
                else if (message_size > 0)
                {
                    ++index;
                    std::stringstream ss;
                    ss << std::this_thread::get_id();
                    send(
                        events[index].data.fd,
                        // buffer,
                        ss.str().c_str(),
                        // message_size,
                        ss.str().size() + 1,
                        MSG_NOSIGNAL);
                }
            }
        }
    }
}

HttpServer::HttpServer()
{
}

void HttpServer::Run()
{
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 3; ++i)
    {
        threads.emplace_back(DoJob);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
}