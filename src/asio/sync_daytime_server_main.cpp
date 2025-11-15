// #include <ctime>
// #include <iostream>
// #include <string>
// #include <asio.hpp>
// #include <spdlog/spdlog.h>

// using asio::ip::tcp;

// /**
//  * Synchronous TCP daytime server
//  * Following this tutorial: https://think-async.com/Asio/asio-1.36.0/doc/asio/tutorial/tutdaytime2.html
//  */

// //  helper function to construct the daytime string to send to a client
// std::string make_daytime_string()
// {
//     spdlog::info("Sending daytime data...");
//     std::time_t now = std::time(0);
//     return std::ctime(&now);
// }

// int main()
// {
//     try
//     {
//         asio::io_context io_context;

//         // the 'acceptor' object listens for new connections
//         // convention is to listen on TCP port 13 for daytime servers
//         tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));

//         spdlog::info("Listening on port 13");

//         // this is an "iterative" server -> handles ONLY one connection at a time
//         for (;;)
//         {
//             tcp::socket socket(io_context);
//             acceptor.accept(socket); // accepts a new connection from a client into the given socket

//             // once client connects, send current daytime data
//             std::string response = make_daytime_string();
//             std::error_code ignored_error;
//             // "write" will write data to a stream
//             // in our case write the response (as a buffer) to the socket we're using
//             asio::write(socket, asio::buffer(response), ignored_error);
//         }
//     }
//     catch (std::exception &e)
//     {
//         std::cerr << e.what() << std::endl;
//     }

//     return 0;
// }