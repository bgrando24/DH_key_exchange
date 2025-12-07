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
    void init()
    {
        while (true)
        {
            // get values
            this->userListeningPort_ = InputHandler::getIntInput(3000, 4000, "Enter the port number you wish to listen on, between 3000 and 4000: ");
            this->remotePeerHost_ = InputHandler::getLineInput("Enter the host address of the remote peer to connect with: ");
            this->remotePeerPort_ = InputHandler::getIntInput(3000, 4000, "Enter the port number for the remote peer, between 3000 and 4000: ");

            std::cout << "----- The following configuration is active -----" << std::endl;
            std::cout << "Your listening port: " << this->userListeningPort_ << std::endl;
            std::cout << "Remote peer address: " << this->remotePeerHost_ << ":" << this->remotePeerPort_ << std::endl;
            std::cout << "----------\n"
                      << std::endl;
            std::string answer;
            while (true)
            {
                answer = InputHandler::getLineInput("Do you wish to proceed (Y/N)?");
                if (answer == "y" || answer == "Y" || answer == "n" || answer == "N")
                    break;
            }

            if (answer == "y" || answer == "Y")
                break;
        }

        this->mode = CurrentMode::KEY_EXCHANGE_READY;

        // spdlog::info("[DHKEClient] init() - Values provided: ");
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