#ifndef CONTEXT_HANDLER_HPP
#define CONTEXT_HANDLER_HPP

#include <boost/multiprecision/cpp_int.hpp>
#include <spdlog/spdlog.h>

#include "key_gen.hpp"
#include "participant.hpp"

/**
  The DHKE Context Manager class provides a standardised environment for the key exchange participants
 */
class DHKEContextManager
{
private:
  // bob is participant 1, alice is participant 2
  DHKEParticipant *bob;
  DHKEParticipant *alice;
  boost::multiprecision::cpp_int publicPrime;
  int publicGenerator;
  boost::multiprecision::cpp_int sharedSecretKey;

public:
  DHKEContextManager(DHKEParticipant bob, DHKEParticipant alice)
  {
    this->bob = &bob;
    this->alice = &alice;
  }

  /**
   * Generates the required public parameters, along with the secret keys for each participant
   * @param bitLength The bit length of the public prime number parameter
   */
  void generateParameters(size_t bitLength)
  {
    spdlog::info("Running DHKEContextManager generateParameters");
    this->publicPrime = KeyGenerator::getPrimeNumber(bitLength);
    // pick either '2' or '5' at random as the generator
    this->publicGenerator = KeyGenerator::getLargeRandomInt(1, 10) % 2 == 0 ? 2 : 5;
    spdlog::info("[generatePublicParameters] Generated params -> Generator: {} - Prime (first 10 digits): {}",
                 this->publicGenerator,
                 this->publicPrime.str().substr(0, 10));

    // give participants the generator and prime
    this->bob->setPublicGenerator(this->publicGenerator);
    this->bob->setPublicPrime(this->publicGenerator);

    this->alice->setPublicGenerator(this->publicGenerator);
    this->alice->setPublicPrime(this->publicGenerator);

    // generate secret keys
    this->bob->setPrivateKey(KeyGenerator::getLargeRandomInt(2, bitLength - 1));
    this->alice->setPrivateKey(KeyGenerator::getLargeRandomInt(2, bitLength - 1));
  }

  /**
   * Performs step 1 of the exchange: Bob and Alice combine the public parameters with their personal secret keys
   */
  void step1()
  {
    spdlog::info("Running DHKEContextManager step 1...");
    // generate keys for both participants
    this->bob->step1();
    this->alice->step1();
  }

  /**
   * Performs step 2 of the exchange: swap intermediary keys, generate final shared secret key
   */
  void step2()
  {
    spdlog::info("Running DHKEContextManager step 2...");
    // grab alice's key and input for bob step 2, and visa versa
    this->bob->step2(this->alice->getStep1Key());
    this->alice->step2(this->bob->getStep1Key());
  }

  bool validateSharedSecret()
  {
    std::cout << "Are the two shared secrets the same? - ";
    if (this->bob->getSharedSecret() == this->alice->getSharedSecret())
    {
      std::cout << "TRUE" << std::endl;
      return true;
    }
    else
    {
      std::cout << "FALSE" << std::endl;
      return false;
    }
  }
};

#endif