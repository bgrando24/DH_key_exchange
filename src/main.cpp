#include <iostream>
#include <spdlog/spdlog.h>
#include <boost/multiprecision/cpp_int.hpp>

#include "dhke/key_gen.hpp"
#include "dhke/key_exchanger.hpp"

int main()
{
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

    // a small integer (i.e. 2 or 5) is appropriate as the generator
    int publicGenerator = 5;
    // generate required public prime, and private keys
    boost::multiprecision::cpp_int prime512 = KeyGenerator::getPrimeNumber(512);
    boost::multiprecision::cpp_int bobPrivateKey = KeyGenerator::getLargeRandomInt(2, 500);
    boost::multiprecision::cpp_int alicePrivateKey = KeyGenerator::getLargeRandomInt(2, 500);

    // create Alice and Bob
    DHKEParticipant Bob = DHKEParticipant(publicGenerator, prime512, bobPrivateKey, "Bob");
    DHKEParticipant Alice = DHKEParticipant(publicGenerator, prime512, alicePrivateKey, "Alice");

    Bob.step1();
    Alice.step1();
    return 0;
}