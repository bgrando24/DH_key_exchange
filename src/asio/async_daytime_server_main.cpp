#include <ctime>
#include <iostream>
#include <string>
#include <asio.hpp>
#include <spdlog/spdlog.h>

using asio::ip::tcp;

/**
 * Asynchronous TCP daytime server
 * Following this tutorial: https://think-async.com/Asio/asio-1.36.0/doc/asio/tutorial/tutdaytime3.html
 */

//  helper function to construct the daytime string to send to a client
std::string make_daytime_string()
{
    spdlog::info("Sending daytime data...");
    std::time_t now = std::time(0);
    return std::ctime(&now);
}

/**
 * The tcp_connection class helps handle a specific tcp client connection
 * Using a shared_ptr and enable_shared_from_this keeps the object alive as along as an operation refers to it
 */
class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    static pointer create(asio::io_context &io_context)
    {
        return pointer(new tcp_connection(io_context));
    }

    tcp::socket &socket()
    {
        return socket_;
    }

    // in the start function, we async_write to send data to the client and ensure the whole data block is sent
    void start()
    {
        message_ = make_daytime_string();
        asio::async_write(socket_, asio::buffer(message_),
                          std::bind(&tcp_connection::handle_write, shared_from_this(),
                                    asio::placeholders::error,
                                    asio::placeholders::bytes_transferred));
    }

private:
    tcp::socket socket_;
    std::string message_;

    tcp_connection(asio::io_context &io_context)
        : socket_(io_context)
    {
        ;
    }

    void handle_write(const std::error_code & /*error*/,
                      size_t /*bytes_transferred*/)
    {
        ;
    }
};

/**
 * tcp_server class provides a server object to handle incoming client connection
 */
class tcp_server
{
public:
    // constructor accepts an asio context, initialises an acceptor
    tcp_server(asio::io_context &io_context)
        : io_context_(io_context),
          acceptor_(io_context, tcp::endpoint(tcp::v4(), 13))
    {
        start_accept();
    }

private:
    asio::io_context &io_context_;
    tcp::acceptor acceptor_;

    // start_accept method creates a socket and inits an async accept operation to wait for new connections
    void start_accept()
    {
        // new socket
        tcp_connection::pointer new_connection = tcp_connection::create(io_context_);
        // ready an acceptor for the new connection
        acceptor_.async_accept(new_connection->socket(),
                               std::bind(&tcp_server::handle_accept, this, new_connection, asio::placeholders::error));
    }

    // handle_accept is called when the asynchronous accept operation
    //      initiated by start_accept finishes -> serves client request, then calls start_accept
    //      to initiate the next accept operation
    void handle_accept(tcp_connection::pointer new_connection, const std::error_code &error)
    {
        spdlog::info("New connection accepted");
        if (!error)
        {
            new_connection->start();
        }

        start_accept();
    }
};

int main()
{
    try
    {
        asio::io_context io_context;
        // server object handles incoming client connections
        tcp_server server(io_context);
        // running the io_context object will perform async operations for us
        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}