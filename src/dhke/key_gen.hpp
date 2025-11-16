#ifndef KEY_GEN
#define KEY_GEN

#include <random>
#include <limits>
// the Boost 'multiprecision' library helps with handling numbers of size >64 bits cross-platform
#include <boost/multiprecision/cpp_int.hpp>
// boost also has an implementation of the miller-rabin primality test - I use this library as to not detract from the main focus of this assignment
#include <boost/multiprecision/miller_rabin.hpp>
#include <spdlog/spdlog.h>

/**
 * Handles functionality related to generating a secret key for participants of the DHKE
 */
class SecretKeyGenerator
{
private:
    // int generatorNumber_;

    /**
     * Generate a random number of a set bit size, intended to be used for generating candidate prime numbers
     *
     * @param bitLength (DEFAULT: 64) The desired BIT (not digit) length of the generated prime number (e.g. 64 bit, 128 bit, 256 bit)
     * @returns boost::multiprecision::cpp_int An integer of the specified bit length
     */
    static boost::multiprecision::cpp_int getCandidateNumber(size_t bitLength = 64)
    {
        if (bitLength == 0)
            throw std::invalid_argument("bitLength must be >= 1");

        /* C++ standard <random> library requires a seed source (i.e. random device),
            a pseudo-RNG algorithm (i.e. mt19937 for 64 bit),
            and a desired distribution to map the RNG output to (i.e. uniform, can also be normal, binomial)
        */
        std::random_device rd;
        std::mt19937_64 randIntGenerator(rd());
        std::uniform_int_distribution<std::uint64_t> randDistribution(0, std::numeric_limits<std::uint64_t>::max());

        // because C++ natively deals with int sizes less than 64 bit, we break the random number generation up into
        //      64 bit chunks and 'stitch' the number together later
        constexpr std::size_t bitsPerChunk = 64;
        /* because C++ int division truncates (i.e. 11/4 would return 2) we need to ensure we have more than enough
            'chunks' of 64 bits. Adding the 'bitsPerChunk' ensures we will have enough chunks, and subtracting 1
            ensures we don't have an unused chunk in the case where there's no remainder
        */
        std::size_t chunks = (bitLength + bitsPerChunk - 1) / bitsPerChunk;

        boost::multiprecision::cpp_int candidateValue = 0;

        // build number from bit chunks
        for (std::size_t i = 0; i < chunks; i++)
        {
            // left-shift the current chunk, essentially adding a 'blank' chunk to the right with all zeros
            candidateValue <<= bitsPerChunk;
            // logical OR will essentially input the RNG output into the new blank chunk,
            //      because OR will insert a 1 if the RNG value at that bit place is 1, otherwise insert zero
            candidateValue |= randDistribution(randIntGenerator);
        }

        // ensure the candidate number is >= bitLength, if any bits overshoot keep only the lowest bit values
        std::size_t totalBits = chunks * bitsPerChunk;
        if (totalBits > bitLength)
        {
            // mask is created by making a binary value of size 'bitLength', subtracting one will set all bit values
            //      less than bitLength index (e.g. bit position 64) to a value of 1
            boost::multiprecision::cpp_int mask = (boost::multiprecision::cpp_int(1) << bitLength) - 1;
            // for the mask, values above the target bit length will be all zeros, so applying the bitwise AND operator
            //      will zero out any values above the target bit length to zero as well
            candidateValue &= mask;
        }

        // force the left-most (highest) bit to be 1, if it happens to be zero then the binary value isn't truly 'bitLength' in size
        candidateValue |= (boost::multiprecision::cpp_int(1) << bitLength - 1);

        // any prime number above 2 must be an odd number (otherwise it's divisible by 2 and not prime),
        //      we can make the candidate value odd by setting the least significant bit (right-most) to 1
        candidateValue |= 1;

        return candidateValue;
    }

public:
    // SecretKeyGenerator()
    // {
    //     ;
    // }

    /**
     * The methodology this function uses is to continuously generate a random integer of a specified BIT length,
     * check if it's prime using the Miller-Rabin primality test, and continue until a prime number is found.
     *
     * @param bitLength (DEFAULT: 64) The desired BIT (not digit) length of the generated prime number (e.g. 64 bit, 128 bit, 256 bit)
     * @returns boost::multiprecision::cpp_int A prime number of the specified bit length
     */
    static boost::multiprecision::cpp_int getPrimeNumber(size_t bitLength = 64)
    {
        boost::multiprecision::cpp_int candidateValue;

        while (true)
        {
            // get candidate prime number (NOT yet determined if actually prime)
            candidateValue = getCandidateNumber(bitLength);
            // from the Boost docs, 25 trials is recommended:
            //  https://www.boost.org/doc/libs/latest/libs/multiprecision/doc/html/boost_multiprecision/tut/primetest.html
            if (boost::multiprecision::miller_rabin_test(candidateValue, 25))
                break;
        }
        spdlog::info("[SecretKeyGenerator::getPrimeNumber] prime number generated: {}", candidateValue.str());
        return candidateValue;
    }
};

#endif