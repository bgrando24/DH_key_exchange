#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <spdlog/spdlog.h>
#include "participant.hpp"
#include "../InputHandler.hpp"

/**
 * The DHKEClient class provides functionality related to running the application in 'client' mode, where a user is one of
 *      two peers in P2P communication using the Diffie-Hellman Key Exchange protocol.
 *
 * This provides the user with the ability to connect with another peer, perform the key exchange, and communicate via encrypted chat.
 *
 * The DHKEClient class extends the DHKEParticipant class.
 */
class DHKEClient : public DHKEParticipant
{
private:
    // remote peer host address
    std::string remotePeerHost_;
    std::string remotePeerPort_;
    // user's port to listen on
    std::string userListeningPort_;

public:
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
        std::string listeningPort,
        std::string remotePeerHost,
        std::string remotePeerPort) : DHKEParticipant(name)
    {
        this->name = name;
        this->userListeningPort_ = listeningPort;
        this->remotePeerHost_ = remotePeerHost;
        this->remotePeerPort_ = remotePeerPort;
    }
};

#endif