// #include <iostream>
// #include <functional>
// #include <spdlog/spdlog.h>
// #include <asio.hpp>
// // #include <nlohmann/json.hpp>

// // this completion handler needs to access the timer object
// // therefore, we pass it a POINTER to the timer object, and a counter to know when to stop
// void print(const std::error_code &, asio::steady_timer *t, int *count)
// {
//     if (*count < 5)
//     {
//         std::cout << *count << std::endl;
//         ++(*count);
//         // we need to manually 'move' the expiry time along
//         // ensures that the timer does not drift away from the whole-second mark due to any delays in processing the handler
//         t->expires_at(t->expiry() + asio::chrono::seconds(1));
//         // then start new async wait on the timer
//         // the std::bind function is used to associate the extra params with the completion handler
//         // the async_await function expects a completion handler object with the signature "void(const std::error_code&)"
//         // thus, the bind function converts the handler to match this signature
//         t->async_wait(std::bind(
//             print,
//             asio::placeholders::error, // named placeholder for error object passed to handler, we must specify the arguments that match the handler's params list which includes this
//             t,
//             count));
//     }
// }

// int main()
// {
//     spdlog::info("Starting");
//     // IO context - provides access to IO functionality
//     asio::io_context io;

//     // counter so we can stop the timer loop as a certain number of repeats
//     int counter = 0;

//     // core asio classes that use IO take an executor as first argument
//     // in this case, the 'timer' functionality
//     asio::steady_timer t(io, asio::chrono::seconds(5)); // second argument sets timer to expire in 5 seconds

//     // blocking wait on the timer
//     // t.wait();

//     spdlog::info("before async wait");

//     // non-blocking (async) wait call -> this is called AFTER the wait is done
//     // the function passed to async_wait is the completion handler -> completion handler is passed a completion token
//     // 'other' execution will continue, but the 'print' function isn't called until the timer expires
//     // t.async_wait(&print);

//     // this time, we bind the additional parameters to the print function
//     t.async_wait(std::bind(
//         print,
//         asio::placeholders::error,
//         &t, // dont forget we need to pass by reference to use the original object
//         &counter));

//     spdlog::info("after async wait");

//     // asio provides guarantee that completion handlers are only called from threads that are currently calling the "run()" io context function
//     // therefore we need to explicitly call that function, otherwise the completion handler is never invoked
//     io.run();

//     spdlog::info("Timer finished!");
//     std::cout << "Final count is " << counter << std::endl;

//     return 0;
// }