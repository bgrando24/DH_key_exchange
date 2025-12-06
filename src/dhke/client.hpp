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
    int remotePeerPort_;
    // user's port to listen on
    int userListeningPort_;

    enum CurrentMode
    {
        LISTEN_PORT_REQUIRED,      // User's listening port not yet provided
        REMOTE_PEER_ADDR_REQUIRED, // Remote peer's host addr and port number not specified yet
        KEY_EXCHANGE_READY,        // Above details are known but awaiting user to confirm proceeding
        USER_READY,                // User flagged themselves as ready for exchange
        DHKE_ACTIVE,               // Key exchange actively happening
        CHAT,                      // Keys exchanged, client moved into chat mode
        DEMO                       // (TBA) demo mode
    };
    CurrentMode mode;

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
        int listeningPort,
        std::string remotePeerHost,
        int remotePeerPort) : DHKEParticipant(name)
    {
        this->name = name;
        this->userListeningPort_ = listeningPort;
        this->remotePeerHost_ = remotePeerHost;
        this->remotePeerPort_ = remotePeerPort;
    }

    void showMenu()
    {
        std::cout << "---------- MENU ----------" << std::endl;
        switch (this->mode)
        {
        case CurrentMode::LISTEN_PORT_REQUIRED:
            /* code */
            break;

        default:
            break;
        }
        // if (this->mode == CurrentMode::LISTEN_PORT_REQUIRED)
        // {
        //     std::cout << "[1] Enter your listening port" << std::endl;
        // }
        // std::cout << "[1] " << std::endl;
    }

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
};

#endif