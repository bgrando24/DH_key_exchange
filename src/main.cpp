#include <iostream>
#include <array>
#include <functional>
#include <spdlog/spdlog.h>
#include <asio.hpp>

/**
 * Synchronous "daytime" TCP client with asio
 */

using asio::ip::tcp;

int main(int argc, char *argv[])
{
    // we need user to specify the server address
    try
    {
        // ensure a host address is supplied
        if (argc != 2)
        {
            spdlog::warn("No client address specified");
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }
        else
        {
            std::cout << "Supplied host: " << argv[1] << std::endl;
        }

        // any program that uses asio needs to have at least one IO execution context
        asio::io_context io_context;

        // we need to convert the supplied server address into a tcp endpoint
        tcp::resolver resolver(io_context);
        // a resolver takes a host name and a service name -> converts to list of endpoints
        // list of endpoints returned using an object "results_type" -> object is a range with begin() and end() members
        tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "daytime");

        // now, create and connect to socket
        // the connect() function will automatically handle both ipv4 and ipv6
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        // connection now open, we need to read the response
        // we use an array to hold the received data
        // asio::buffer() automatically determines the size of the array to help prevent overflows
        for (;;)
        {
            std::array<char, 128> buff;
            std::error_code error;

            size_t len = socket.read_some(asio::buffer(buff), error);

            // when server closes connection, the read_some function will exit with eof error
            if (error == asio::error::eof)
                break;
            else if (error)
                throw std::system_error(error);

            std::cout.write(buff.data(), len);
            std::cout << buff.data() << std::endl;
        }
    }
    catch (std::exception &e)
    {
        spdlog::error("CAUGHT ERROR");
        std::cerr << e.what() << std::endl;
    }

    return 0;
};