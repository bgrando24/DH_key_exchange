#include <iostream>
#include <functional>
#include <spdlog/spdlog.h>
#include <asio.hpp>
// #include <nlohmann/json.hpp>

// this completion handler needs to access the timer object
// therefore, we pass it a POINTER to the timer object, and a counter to know when to stop
void print(const std::error_code &, asio::steady_timer *t, int *count)
{
    if (*count < 5)
    {
        std::cout << *count << std::endl;
        ++(*count);
        // we need to manually 'move' the expiry time along
        t->expires_at(t->expiry()() + asio::chrono::seconds(1));
    }
}

int main()
{
    spdlog::info("Starting");
    // IO context - provides access to IO functionality
    asio::io_context io;

    // core asio classes that use IO take an executor as first argument
    // in this case, the 'timer' functionality
    asio::steady_timer t(io, asio::chrono::seconds(5)); // second argument sets timer to expire in 5 seconds

    // blocking wait on the timer
    // t.wait();

    spdlog::info("before async wait");

    // non-blocking (async) wait call -> this is called AFTER the wait is done
    // the function passed to async_wait is the completion handler -> completion handler is passed a completion token
    // 'other' execution will continue, but the 'print' function isn't called until the timer expires
    t.async_wait(&print);

    spdlog::info("after async wait");

    // asio provides guarantee that completion handlers are only called from threads that are currently calling the "run()" io context function
    // therefore we need to explicitly call that function, otherwise the completion handler is never invoked
    io.run();

    spdlog::info("Timer finished!");

    return 0;
}