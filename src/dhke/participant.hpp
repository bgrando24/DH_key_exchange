#include <string>
#include <iostream>
#include <spdlog/spdlog.h>
#include <boost/multiprecision/cpp_int.hpp>

/**
 * The DHKEParticipant class handles functionality required for a client to participate in the DHKE process.
 *
 * Throughout the comments for this class, the pseudonyms Bob and Alice will be used.
 *  Bob will refer to 'this' current instance (us), and Alice will refer to the other participant we intend to communicate with.
 */
class DHKEParticipant
{
private:
    // required parameters for each participant
    boost::multiprecision::cpp_int publicGenerator_;
    boost::multiprecision::cpp_int publicPrime_;
    boost::multiprecision::cpp_int privateKey_;
    // name of participant, for observation purposes
    std::string name_;
    // this tracks what step of the key exchange the participant is on
    enum ExchangeSteps
    {
        START,
        COMPUTE_PUBLIC_KEY,
        FINAL_KEY_GENERATED
    };
    ExchangeSteps currentStep;

public:
    /**
     * @param publicGenerator Shared generator parameter
     * @param publicPrime Shared large prime number parameter
     * @param privateKey_ The participant's individual private key
     */
    DHKEParticipant(
        int publicGenerator,
        boost::multiprecision::cpp_int publicPrime,
        boost::multiprecision::cpp_int privateKey,
        std::string name)
    {
        this->publicGenerator_ = publicGenerator;
        this->publicPrime_ = publicPrime;
        this->privateKey_ = privateKey;
        this->name_ = name;
        // flag that the required participant parameters are ready
        currentStep = ExchangeSteps::START;
    }

    /**
     * The 'step 1' function performs the initial combination of the participant's secret key and the public parameters.
     * For public prime = p, generator = g, and private key = a, the following is comupted: g^a mod p
     * @returns boost::multiprecision::cpp_int - The resulting combined key
     */
    boost::multiprecision::cpp_int step1()
    {
        spdlog::info("Starting DHKEParticipant step 1...");

        // from the Boost documentation: https://www.boost.org/doc/libs/latest/libs/multiprecision/doc/html/boost_multiprecision/tut/gen_int.html
        // for params b,p,m the 'powm' function returns b^p % m
        boost::multiprecision::cpp_int value = boost::multiprecision::powm(
            this->publicGenerator_,
            this->privateKey_,
            this->publicPrime_);

        // flag first step as being completed
        this->currentStep = COMPUTE_PUBLIC_KEY;
        spdlog::info("Step 1 value generated for {}: ", this->name_);
        std::cout << value << std::endl;
        return value;
    };

    /**
     * The 'step 2' function combines the received public key from Alice (which is a combination of the public params and
     *      Alice's private key), with Bob's (our) secret key
     * @param publicKey The public key received from the other participant
     * @returns boost::multiprecision::cpp_int - The final shared secret key
     */
    boost::multiprecision::cpp_int step2(boost::multiprecision::cpp_int publicKey)
    {
        spdlog::info("Starting DHKEParticipant step 2...");

        // for public key = B, private key = a, and public prime = p, to compute the shared secret key (s) we use:
        //      s = B^a mod p
        boost::multiprecision::cpp_int sharedSecret = boost::multiprecision::powm(
            publicKey,
            this->privateKey_,
            this->publicPrime_);

        // flag the final step as being completed
        this->currentStep = FINAL_KEY_GENERATED;
        spdlog::info("Step 2 shared secret generated for {}: ", this->name_);
        std::cout << sharedSecret << std::endl;
        return sharedSecret;
    }
};