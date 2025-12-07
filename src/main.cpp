#include <iostream>
#include <spdlog/spdlog.h>
#include <boost/multiprecision/cpp_int.hpp>

#include "dhke/key_gen.hpp"
#include "dhke/participant.hpp"
#include "dhke/client.hpp"
#include "dhke/context_handler.hpp"

void printNetworkUsage()
{
    std::cout << "Network mode usage:\n";
    std::cout << "  Listener: app listen <name> <expected_peer_name> <listen_port> <auth_secret>\n";
    std::cout << "  Connector: app connect <name> <expected_peer_name> <listen_port> <peer_host> <peer_port> <auth_secret>\n";
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    spdlog::info("Starting DH key demo");

    // we have two network modes: 'listen', and 'connect'
    // The 'listen' mode waits for a peer to connect, listening on the specified port
    // The 'connect' mode attempts to connect to a listening peer
    if (argc >= 2)
    {
        std::string role = argv[1];
        if (role == "listen")
        {
            if (argc != 6)
            {
                printNetworkUsage();
                return 1;
            }

            // extract CLI arguments
            std::string name = argv[2];
            std::string expectedPeerName = argv[3];
            int listenPort = std::stoi(argv[4]);
            std::string authSecret = argv[5];
            DHKEClient listener(name, listenPort, "localhost", 0);
            bool ok = listener.performListenerHandshake(authSecret, expectedPeerName);
            return ok ? 0 : 1;
        }
        else if (role == "connect")
        {
            if (argc != 8)
            {
                printNetworkUsage();
                return 1;
            }
            // extract CLI arguments
            std::string name = argv[2];
            std::string expectedPeerName = argv[3];
            int listenPort = std::stoi(argv[4]);
            std::string peerHost = argv[5];
            int peerPort = std::stoi(argv[6]);
            std::string authSecret = argv[7];
            DHKEClient connector(name, listenPort, peerHost, peerPort);
            bool ok = connector.performConnectorHandshake(authSecret, expectedPeerName);
            return ok ? 0 : 1;
        }
        else
        {
            printNetworkUsage();
            return 1;
        }
    }

    return 0;
}
