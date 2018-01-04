#include <iostream>

#include <boost/program_options.hpp>

#include "HttpServer/HttpServer.h"

int main(int argc, char* argv[])
{
    boost::program_options::options_description command_line_options_descriptions(
        "Allowed options");

    command_line_options_descriptions.add_options()
        ("help,h", "show help")
        ("ip", boost::program_options::value<std::string>()->required(), "ip address")
        ("port", boost::program_options::value<uint16_t>()->required(), "port")
        ("threads", boost::program_options::value<uint16_t>()->required(), "threads amount")
        ("unzipped", "is input data unzipped")
        ("input", boost::program_options::value<std::string>()->required(), "path to data")
        ("local", "is launch at local pc");

    boost::program_options::variables_map command_line_options;
    store(
        parse_command_line(
            argc,
            argv,
            command_line_options_descriptions),
        command_line_options);

    try
    {
        bool is_data_unzipped = false;
        bool is_local_launch = false;
        std::string path_to_data("tmp/data/data.zip");
        std::string ip_address("127.0.0.1");
        uint16_t port = 80;
        uint16_t threads_amount = 4;

        if (command_line_options.count("help"))
        {
            std::cout << command_line_options_descriptions << std::endl;
            return 0;
        }

        if (command_line_options.count("ip"))
        {
            ip_address =
                command_line_options["ip"].as<std::string>();
        }

        if (command_line_options.count("port"))
        {
            port =
                command_line_options["port"].as<uint16_t>();
        }

        if (command_line_options.count("threads"))
        {
            threads_amount =
                command_line_options["threads"].as<uint16_t>();
        }

        if (command_line_options.count("unzipped"))
        {
            is_data_unzipped = true;
        }

        if (command_line_options.count("input"))
        {
            path_to_data =
                command_line_options["input"].as<std::string>();
        }
        
        if (command_line_options.count("local"))
        {
            is_local_launch = true;
        }
        
        HttpServer http_server(
            ip_address,
            port,
            is_data_unzipped ? HttpServer::LoadedDataType::Unzipped : HttpServer::LoadedDataType::Zipped,
            path_to_data,
            threads_amount,
            is_local_launch);

        http_server.Run();
    }
    catch (const boost::program_options::error& program_options_error)
    {
        std::cerr << program_options_error.what() << std::endl;
        std::cout << program_options_error.what() << std::endl;
        return -1;
    }
    catch (std::exception const &e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << e.what() << std::endl;
        return -1;
    }
}
