#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sstream>
#include <functional>
#include <asio.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <spdlog/spdlog.h>
#include "participant.hpp"
#include "key_gen.hpp"
#include "../InputHandler.hpp"

/**
 * The DHKEClient class extends the DHKEParticipant class. DHKEClient provides functionality related to network communication
 *      and 'handshake' establishment with another peer utilising the Diffie-Hellman key exchange protocol.
 */
class DHKEClient : public DHKEParticipant
{
private:
    // remote peer host address
    std::string remotePeerHost_;
    int remotePeerPort_;
    // user's port to listen on
    int userListeningPort_;

    /**
     * Helper method to format the payload sent over the P2P communication
     * @param prime The public prime number
     * @param generator The public generator
     * @param publicKey The participant's public key
     * @param role The participant's role in the exchange (LISTENER or CONNECTOR)
     * @param senderId The sender's identity
     * @param receiverId The receiver's identity
     * @returns The formatted payload string
     */
    static std::string buildPayload(
        const boost::multiprecision::cpp_int &prime,
        int generator,
        const boost::multiprecision::cpp_int &publicKey,
        const std::string &role,
        const std::string &senderId,
        const std::string &receiverId)
    {
        std::ostringstream oss;
        oss << prime << "|" << generator << "|" << publicKey << "|" << role << "|" << senderId << "|" << receiverId;
        return oss.str();
    }

    /**
     * Computes a Message Authentication Code (MAC)
     *
     * @note Currently builds a simplified MAC using std::hash for demonstration purposes
     * @param secret The secret key used for MAC computation
     * @param payload The payload to be authenticated
     * @returns The computed MAC as a hexadecimal string
     */
    static std::string computeMac(const std::string &secret, const std::string &payload)
    {
        std::hash<std::string> hasher;
        std::ostringstream oss;
        /**
         * What's happening below:
         *  1. 'oss' is essentially the buffer we want to write our data to. In C++, the stream insertion operator, "<<"
         *           allows for passing data into a stream buffer object
         *  2. 'std::hex' is an I/O manipulator, meaning it converts any supplied data into hexadecimal base
         *  3. The hasher() function uses std::hash, which computes a hash of the supplied data, in our case the secret and the payload concatenated
         *
         * In summary: hashed secret + payload value -> converted into hexadecimal -> passed into the buffer, then returned as a regular string value
         */
        oss << std::hex << hasher(secret + "|" + payload);
        return oss.str();
    }

    /**
     * Helper method to derive and format the confirmation tag for the key exchange
     * @param shared The shared secret
     * @param role The participant's role in the exchange (LISTENER or CONNECTOR)
     * @param self The participant's identity
     * @param peer The peer's identity
     * @returns The confirmation tag as a hexadecimal string
     */
    static std::string deriveConfirmTag(const boost::multiprecision::cpp_int &shared, const std::string &role, const std::string &self, const std::string &peer)
    {
        return computeMac(shared.str(), "CONFIRM|" + role + "|" + self + "|" + peer);
    }

    /**
     * Helper method to derive session MAC key from the shared secret
     * @param shared The shared secret
     * @returns The session key as a hexadecimal string
     */
    static std::string deriveSessionKey(const boost::multiprecision::cpp_int &shared)
    {
        return computeMac(shared.str(), "SESSION_KEY");
    }

    /**
     * Helper method to create a shortened hash representation of a big integer
     * @param value The big integer value
     * @returns The shortened hash as a hexadecimal string
     */
    static std::string shortHash(const boost::multiprecision::cpp_int &value)
    {
        std::hash<std::string> hasher;
        std::ostringstream oss;
        oss << std::hex << hasher(value.str());
        return oss.str();
    }

    /**
     * Simple XOR cipher for demonstration purposes, uses the provided key to XOR the data
     * @param data The data to perform XOR on
     * @param key The key used for XOR operation
     * @returns The XOR result
     */
    static std::string xorWithKey(const std::string &data, const std::string &key)
    {
        if (key.empty())
            return {};
        std::string out(data.size(), '\0');
        for (size_t i = 0; i < data.size(); ++i)
        {
            out[i] = data[i] ^ key[i % key.size()];
        }
        return out;
    }

    /**
     * Encodes string data into hexadecimal representation
     * @param data The data to encode
     * @returns The hex-encoded string
     */
    static std::string hexEncode(const std::string &data)
    {
        static const char *hex = "0123456789abcdef"; // valid hex characters
        std::string out;
        // one hex character represents 4 bits, so each full byte requires two hex characters, hence reserving double the memory size of 'data'
        out.reserve(data.size() * 2);
        for (unsigned char c : data)
        {
            // shift right by 4 bits to get the first hex digit, and bitwise AND with 0x0F to get the second hex digit
            out.push_back(hex[c >> 4]);
            out.push_back(hex[c & 0x0F]);
        }
        return out;
    }

    /**
     * Decodes a hex-encoded string back into its original representation
     * @param hexData The hex-encoded string
     * @returns The decoded string
     */
    static std::string hexDecode(const std::string &hexData)
    {
        if (hexData.size() % 2 != 0)
            throw std::invalid_argument("Invalid hex length");
        std::string out;
        // opposite of above, each pair of hex characters represents one byte, so only need half the memory size
        out.reserve(hexData.size() / 2);
        for (size_t i = 0; i < hexData.size(); i += 2)
        {
            // the stoul function converts a substring of two hex characters into an unsigned long integer, base 16
            // we then static_cast it to a char and append to the output string -> giving us the original string value
            unsigned int byte = std::stoul(hexData.substr(i, 2), nullptr, 16);
            out.push_back(static_cast<char>(byte));
        }
        return out;
    }

    /**
     * Helper method for sending a line over ASIO network socket
     * @param socket The ASIO TCP socket object
     * @param line The string to send
     */
    static void sendLine(asio::ip::tcp::socket &socket, const std::string &line)
    {
        asio::write(socket, asio::buffer(line + "\n"));
    }

    /**
     * Helper method for reading a line from ASIO network socket, delimited by a newline character
     * @param socket The ASIO TCP socket object
     * @param buffer The ASIO stream buffer to read into -> where to store the data temporarily
     * @returns The read line as a string
     */
    static std::string readLine(asio::ip::tcp::socket &socket, asio::streambuf &buffer)
    {
        asio::read_until(socket, buffer, "\n");
        std::istream is(&buffer);
        std::string line;
        std::getline(is, line);
        return line;
    }

    /**
     * Validates the DHKE parameters received from the peer. Checks the following conditions:
     *
     * - Prime number is > 3 and odd
     *
     * - Prime number passes Miller-Rabin primality test
     *
     * - Generator is > 1 and < prime
     *
     * - Peer public key is > 1 and < (prime - 1)
     *
     * @param prime The public prime number
     * @param generator The public generator
     * @param peerPartial The peer's public key
     * @returns True if parameters are valid, false otherwise
     */
    static bool validateParameters(const boost::multiprecision::cpp_int &prime,
                                   int generator,
                                   const boost::multiprecision::cpp_int &peerPartial)
    {
        if (prime <= 3 || (prime & 1) == 0)
            return false;
        if (!boost::multiprecision::miller_rabin_test(prime, 10))
            return false;
        if (generator <= 1 || generator >= prime)
            return false;
        if (peerPartial <= 1 || peerPartial >= (prime - 1))
            return false;
        return true;
    }

public:
    // user's name
    std::string name;

    /**
     * @param name The user's name
     */
    DHKEClient(std::string name) : DHKEParticipant(name)
    {
        this->name = name;
    }

    /**
     * @param name The user's name
     * @param listeningPort The port the user want's to listen on
     * @param remotePeerHost The host address of the remote peer
     * @param remotePeerPort The port number the remote peer is listening on
     */
    DHKEClient(
        std::string name,
        int listeningPort,
        std::string remotePeerHost,
        int remotePeerPort) : DHKEParticipant(name)
    {
        this->name = name;
        this->userListeningPort_ = listeningPort;
        this->remotePeerHost_ = remotePeerHost;
        this->remotePeerPort_ = remotePeerPort;
    }

    /**
     * Debugging function to print out the current state of the client
     */
    void printState()
    {
        std::cout << "Name: " << this->name << std::endl;
        std::cout << "Listening Port: " << this->userListeningPort_ << std::endl;
        std::cout << "Remote peer address: " << this->remotePeerHost_ << ":" << this->remotePeerPort_ << std::endl;
    }

    /**
     * Runs through getting the user's listening port, and getting the remote peer's host addr
     */
    // void init()
    // {
    //     while (true)
    //     {
    //         // get values
    //         this->userListeningPort_ = InputHandler::getIntInput(3000, 4000, "Enter the port number you wish to listen on, between 3000 and 4000: ");
    //         this->remotePeerHost_ = InputHandler::getLineInput("Enter the host address of the remote peer to connect with: ");
    //         this->remotePeerPort_ = InputHandler::getIntInput(3000, 4000, "Enter the port number for the remote peer, between 3000 and 4000: ");

    //         std::cout << "----- The following configuration is active -----" << std::endl;
    //         std::cout << "Your listening port: " << this->userListeningPort_ << std::endl;
    //         std::cout << "Remote peer address: " << this->remotePeerHost_ << ":" << this->remotePeerPort_ << std::endl;
    //         std::cout << "----------\n"
    //                   << std::endl;
    //         std::string answer;
    //         while (true)
    //         {
    //             answer = InputHandler::getLineInput("Do you wish to proceed (Y/N)?");
    //             if (answer == "y" || answer == "Y" || answer == "n" || answer == "N")
    //                 break;
    //         }

    //         if (answer == "y" || answer == "Y")
    //             break;
    //     }

    //     this->mode = CurrentMode::KEY_EXCHANGE_READY;
    // }

    // -------------- GETTERS --------------
    std::string getRemotePeerHost()
    {
        return this->remotePeerHost_;
    }

    int getRemotePeerPort()
    {
        return this->remotePeerPort_;
    }

    int getListeningPort()
    {
        return this->userListeningPort_;
    }

    // -------------- SETTERS --------------
    void setRemotePeerHost(std::string address)
    {
        this->remotePeerHost_ = address;
    }

    void setRemotePeerPort(int port)
    {
        this->remotePeerPort_ = port;
    }

    void setListeningPort(int port)
    {
        this->userListeningPort_ = port;
    }

    /**
     * Performs the listener side of the DHKE handshake over the network. For the listener specifically, this involves:
     *
     * - 1. Waiting for a connection from the connector, then generating the DHKE parameters (prime, generator, private key), and computing the partial key
     *
     * - 2. Sending the partial key to the connector, and receiving the connector's partial key
     *
     * - 3. Computing the shared secret, and confirming it with the connector
     *
     * @param authSecret The shared authentication secret for MAC computation
     * @param expectedPeerId The expected identity of the remote peer
     * @param primeBitLength The bit length for the generated prime number (default: 512)
     * @returns True if handshake successful, false otherwise
     */
    bool performListenerHandshake(const std::string &authSecret, const std::string &expectedPeerId, size_t primeBitLength = 512)
    {
        using asio::ip::tcp;
        spdlog::info("[{}] Starting listener handshake on port {}", this->name, this->userListeningPort_);

        try
        {
            // set up asio networking context
            asio::io_context io;
            tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), this->userListeningPort_));
            tcp::socket socket(io);
            acceptor.accept(socket);
            spdlog::info("[{}] Peer connected", this->name);

            // generate parameters: prime, generator, private key, public key
            auto prime = KeyGenerator::getPrimeNumber(primeBitLength);
            int generator = KeyGenerator::getLargeRandomInt(1, 10) % 2 == 0 ? 2 : 5;
            this->setPublicPrime(prime);
            this->setPublicGenerator(generator);
            this->setPrivateKey(KeyGenerator::getLargeRandomInt(2, primeBitLength - 1));

            // perform step 1 to get the partial key
            auto myPublic = this->step1();

            // send initial message to connector
            auto macPayload = buildPayload(prime, generator, myPublic, "LISTENER", this->name, expectedPeerId);
            // compute MAC, send data in expected format to connector
            auto mac = computeMac(authSecret, macPayload);
            sendLine(socket, "ID:" + this->name);
            sendLine(socket, "P:" + prime.str());
            sendLine(socket, "G:" + std::to_string(generator));
            sendLine(socket, "PUB:" + myPublic.str());
            sendLine(socket, "MAC:" + mac);

            // receive peer response
            asio::streambuf buffer;
            boost::multiprecision::cpp_int peerPartial;
            std::string peerMac;
            std::string peerId;
            std::string peerConfirm;

            // expecting 4 lines: PUB, MAC, ID, CONFIRM
            for (int i = 0; i < 4; ++i)
            {
                std::string line = readLine(socket, buffer);
                if (line.rfind("PUB:", 0) == 0)
                {
                    peerPartial = boost::multiprecision::cpp_int(line.substr(4));
                }
                else if (line.rfind("MAC:", 0) == 0)
                {
                    peerMac = line.substr(4);
                }
                else if (line.rfind("ID:", 0) == 0)
                {
                    peerId = line.substr(3);
                }
                else if (line.rfind("CONFIRM:", 0) == 0)
                {
                    peerConfirm = line.substr(8);
                }
            }

            // check received data
            if (peerMac.empty())
            {
                spdlog::error("[{}] Missing MAC from peer", this->name);
                return false;
            }
            if (peerId.empty() || peerId != expectedPeerId)
            {
                spdlog::error("[{}] Unexpected peer identity '{}'", this->name, peerId);
                return false;
            }

            auto expectedMac = computeMac(authSecret, buildPayload(prime, generator, peerPartial, "CONNECTOR", peerId, this->name));
            if (peerMac != expectedMac)
            {
                spdlog::error("[{}] MAC mismatch, aborting handshake", this->name);
                return false;
            }

            if (!validateParameters(prime, generator, peerPartial))
            {
                spdlog::error("[{}] Parameter validation failed", this->name);
                return false;
            }

            // now perform step 2 to compute the complete shared secret, using the peer's partial key
            auto shared = this->step2(peerPartial);
            spdlog::info("[{}] Shared secret hash: {}", this->name, shortHash(shared));

            // confirm peer knows the shared secret
            auto expectedConfirm = deriveConfirmTag(shared, "CONNECTOR", peerId, this->name);
            if (peerConfirm.empty() || peerConfirm != expectedConfirm)
            {
                spdlog::error("[{}] Confirmation tag mismatch", this->name);
                return false;
            }
            // send our confirmation tag back to the connector
            auto myConfirm = deriveConfirmTag(shared, "LISTENER", this->name, peerId);
            sendLine(socket, "CONFIRM:" + myConfirm);

            // demonstration of encrypted message exchange: send and receive two messages
            // this is to show that both parties have derived the same session key from the shared secret, and can encrypt/decrypt communications successfully
            // this uses the simplified XOR cipher
            auto sessionKey = deriveSessionKey(shared);
            std::string msg1 = "Hello from " + this->name + " (listener)";
            sendLine(socket, "ENC:" + hexEncode(xorWithKey(msg1, sessionKey)));
            std::string encReply1 = readLine(socket, buffer);
            if (encReply1.rfind("ENC:", 0) != 0)
            {
                spdlog::error("[{}] Expected encrypted reply", this->name);
                return false;
            }
            std::string reply1 = xorWithKey(hexDecode(encReply1.substr(4)), sessionKey);
            spdlog::info("[{}] Decrypted reply: {}", this->name, reply1);

            std::string msg2 = "Second message from " + this->name;
            sendLine(socket, "ENC:" + hexEncode(xorWithKey(msg2, sessionKey)));
            std::string encReply2 = readLine(socket, buffer);
            if (encReply2.rfind("ENC:", 0) != 0)
            {
                spdlog::error("[{}] Expected second encrypted reply", this->name);
                return false;
            }
            std::string reply2 = xorWithKey(hexDecode(encReply2.substr(4)), sessionKey);
            spdlog::info("[{}] Decrypted second reply: {}", this->name, reply2);
            return true;
        }
        catch (const std::exception &ex)
        {
            spdlog::error("[{}] Listener handshake failed: {}", this->name, ex.what());
            return false;
        }
    }

    /**
     * Performs the connector side of the DHKE handshake over the network. For the connector specifically, this involves:
     *
     * - 1. Connecting to the listener peer
     *
     * - 2. Receiving the listener's parameters and partial key
     *
     * - 3. Sending the connector's partial key
     *
     * - 4. Computing the shared secret, and confirming it with the listener
     *
     * @param authSecret The shared authentication secret for MAC computation
     * @param expectedPeerId The expected identity of the remote peer
     * @param primeBitLength The bit length for the generated prime number (default: 512)
     * @returns True if handshake successful, false otherwise
     */
    bool performConnectorHandshake(const std::string &authSecret, const std::string &expectedPeerId, size_t primeBitLength = 512)
    {
        using asio::ip::tcp;
        spdlog::info("[{}] Starting connector handshake to {}:{}", this->name, this->remotePeerHost_, this->remotePeerPort_);

        try
        {
            // set up asio networking context
            asio::io_context io;
            tcp::socket socket(io);
            tcp::resolver resolver(io);
            auto endpoints = resolver.resolve(this->remotePeerHost_, std::to_string(this->remotePeerPort_));
            asio::connect(socket, endpoints);
            spdlog::info("[{}] Connected to peer", this->name);

            // receive parameters from listener
            asio::streambuf buffer;
            boost::multiprecision::cpp_int prime;
            int generator = 0;
            boost::multiprecision::cpp_int peerPartial;
            std::string peerMac;
            std::string peerId;

            // expecting 5 lines: P, G, PUB, MAC, ID
            for (int i = 0; i < 5; ++i)
            {
                std::string line = readLine(socket, buffer);
                if (line.rfind("P:", 0) == 0)
                {
                    prime = boost::multiprecision::cpp_int(line.substr(2));
                }
                else if (line.rfind("G:", 0) == 0)
                {
                    generator = std::stoi(line.substr(2));
                }
                else if (line.rfind("PUB:", 0) == 0)
                {
                    peerPartial = boost::multiprecision::cpp_int(line.substr(4));
                }
                else if (line.rfind("MAC:", 0) == 0)
                {
                    peerMac = line.substr(4);
                }
                else if (line.rfind("ID:", 0) == 0)
                {
                    peerId = line.substr(3);
                }
            }

            // check received data
            if (peerMac.empty())
            {
                spdlog::error("[{}] Missing MAC from listener", this->name);
                return false;
            }
            if (peerId.empty() || peerId != expectedPeerId)
            {
                spdlog::error("[{}] Unexpected listener identity '{}'", this->name, peerId);
                return false;
            }

            auto expectedMac = computeMac(authSecret, buildPayload(prime, generator, peerPartial, "LISTENER", peerId, this->name));
            if (peerMac != expectedMac)
            {
                spdlog::error("[{}] MAC mismatch, aborting handshake", this->name);
                return false;
            }

            if (!validateParameters(prime, generator, peerPartial))
            {
                spdlog::error("[{}] Parameter validation failed", this->name);
                return false;
            }

            // receive parameters from listener, now generate our own parameters via 'step1()'
            this->setPublicPrime(prime);
            this->setPublicGenerator(generator);
            this->setPrivateKey(KeyGenerator::getLargeRandomInt(2, primeBitLength - 1));
            auto myPublic = this->step1();

            // send MAC + partial key response to listener
            auto macPayload = buildPayload(prime, generator, myPublic, "CONNECTOR", this->name, peerId);
            auto mac = computeMac(authSecret, macPayload);
            sendLine(socket, "ID:" + this->name);
            sendLine(socket, "PUB:" + myPublic.str());
            sendLine(socket, "MAC:" + mac);

            // compute shared secret using listener's partial key
            auto shared = this->step2(peerPartial);
            spdlog::info("[{}] Shared secret hash: {}", this->name, shortHash(shared));
            auto myConfirm = deriveConfirmTag(shared, "CONNECTOR", this->name, peerId);
            // confirm with listener
            sendLine(socket, "CONFIRM:" + myConfirm);

            // receive confirmation from listener
            std::string peerConfirm = readLine(socket, buffer);
            if (peerConfirm.rfind("CONFIRM:", 0) != 0)
            {
                spdlog::error("[{}] Missing confirmation from listener", this->name);
                return false;
            }
            auto confirmValue = peerConfirm.substr(8);
            auto expectedConfirm = deriveConfirmTag(shared, "LISTENER", peerId, this->name);
            if (confirmValue != expectedConfirm)
            {
                spdlog::error("[{}] Confirmation tag mismatch", this->name);
                return false;
            }

            // demo encrypted message exchange (connector replies to two messages)
            auto sessionKey = deriveSessionKey(shared);
            std::string enc1 = readLine(socket, buffer);
            if (enc1.rfind("ENC:", 0) != 0)
            {
                spdlog::error("[{}] Expected encrypted message from listener", this->name);
                return false;
            }
            std::string msg1 = xorWithKey(hexDecode(enc1.substr(4)), sessionKey);
            spdlog::info("[{}] Decrypted message 1: {}", this->name, msg1);
            std::string reply1 = "Ack from " + this->name + " #1";
            sendLine(socket, "ENC:" + hexEncode(xorWithKey(reply1, sessionKey)));

            std::string enc2 = readLine(socket, buffer);
            if (enc2.rfind("ENC:", 0) != 0)
            {
                spdlog::error("[{}] Expected second encrypted message from listener", this->name);
                return false;
            }
            std::string msg2 = xorWithKey(hexDecode(enc2.substr(4)), sessionKey);
            spdlog::info("[{}] Decrypted message 2: {}", this->name, msg2);
            std::string reply2 = "Ack from " + this->name + " #2";
            sendLine(socket, "ENC:" + hexEncode(xorWithKey(reply2, sessionKey)));

            return true;
        }
        catch (const std::exception &ex)
        {
            spdlog::error("[{}] Connector handshake failed: {}", this->name, ex.what());
            return false;
        }
    }
};

#endif
