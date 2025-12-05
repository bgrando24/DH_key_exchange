#include <iostream>
#include <spdlog/spdlog.h>
#include <boost/multiprecision/cpp_int.hpp>

#include "dhke/key_gen.hpp"
#include "dhke/participant.hpp"
#include "dhke/context_handler.hpp"

int main()
{
    spdlog::info("Starting DH key demo");

    // set up key exchange context
    DHKEParticipant Bob = DHKEParticipant("Bob");
    DHKEParticipant Alice = DHKEParticipant("Alice");
    DHKEContextManager manager = DHKEContextManager(Bob, Alice);

    // generate params
    manager.generateParameters(512);
    // run step 1
    manager.step1();
    // run step 2
    manager.step2();
    // validate
    manager.validateSharedSecret();

    return 0;
}