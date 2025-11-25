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

// testing large prime number generation
// boost::multiprecision::cpp_int prime64 = KeyGenerator::getPrimeNumber(/*64*/);
// boost::multiprecision::cpp_int prime128 = KeyGenerator::getPrimeNumber(128);
// boost::multiprecision::cpp_int prime256 = KeyGenerator::getPrimeNumber(256);
// boost::multiprecision::cpp_int prime512 = KeyGenerator::getPrimeNumber(512);
// boost::multiprecision::cpp_int prime1024 = KeyGenerator::getPrimeNumber(1024);
// boost::multiprecision::cpp_int prime2048 = KeyGenerator::getPrimeNumber(2048);

// testing large random int generation
// KeyGenerator::getLargeRandomInt(2, 10);
// KeyGenerator::getLargeRandomInt(2, 100);
// KeyGenerator::getLargeRandomInt(2, 1000);
// KeyGenerator::getLargeRandomInt(2, 10000);