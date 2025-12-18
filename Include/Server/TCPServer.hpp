#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>


#include <thread>
#include <vector>
#include "Server/Game.hpp"

#include "Protocole/ProtocoleTCP.hpp"
#include "Server/Game.hpp"
#include "Clock.hpp"

/**
 * @file TCPServer.hpp
 * @brief Handles TCP connections for player login and handshake.
 */

/**
 * @class TCPServer
 * @brief Server class managing TCP connections.
 *
 * This class accepts incoming TCP connections, performs the initial handshake
 * (username exchange), assigns a player ID and UDP port, and then hands off
 * the player to the game logic.
 */
class TCPServer {
public:
    /**
     * @brief Construct a new TCPServer object.
     * @param port The TCP port to listen on.
     * @param game Reference to the Game instance.
     * @param clock Reference to the shared Clock.
     */
    TCPServer(int port, Game& game, Clock& clock);

    /**
     * @brief Destroy the TCPServer object.
     */
    ~TCPServer();

    /**
     * @brief Starts the server and the acceptance thread.
     */
    void start();

    /**
     * @brief Stops the server and joins threads.
     */
    void stop();

private:
    void acceptLoop();
    void handleClient(int clientSock);

    int _sockfd; /**< Server socket file descriptor */
    bool _running; /**< Running state flag */
    Game& _game; /**< Reference to the game logic */
    uint32_t _nextPlayerId = 1; /**< Counter for assigning unique player IDs */

    std::thread _acceptThread; /**< Thread for accepting new connections */
    std::vector<std::thread> _clientThread; /**< Threads for handling individual client handshakes */
    const Clock& _clock; /**< Reference to the server clock */
};

#endif
