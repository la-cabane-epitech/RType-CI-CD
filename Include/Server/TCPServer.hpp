/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPServer
**
** This header defines the TCPServer class, responsible for handling
** incoming TCP connections from R-Type clients. The server accepts
** client connections, performs the handshake using the TCP protocol,
** assigns a unique player ID, and provides the UDP port for gameplay.
*/

#ifndef TCPSERVER_HPP_
    #define TCPSERVER_HPP_

    #include <cstdint>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <iostream>
    #include <thread>
    #include <cstring>
    #include <vector>

#include "ProtocoleTCP.hpp"

/**
 * @class TCPServer
 * @brief Manages TCP connections for the R-Type server.
 *
 * Responsibilities:
 * - Listen on a specified TCP port.
 * - Accept multiple client connections (multi-threaded).
 * - Perform handshake using ConnectRequest / ConnectResponse / ErrorResponse.
 * - Assign unique player IDs and UDP ports for gameplay.
 */
class TCPServer {
public:
    /**
     * @brief Constructor: initializes the TCP server on a given port.
     * @param port TCP port to bind and listen on.
     */
    TCPServer(int port);

    /**
     * @brief Destructor: stops the server and closes the socket.
     */
    ~TCPServer();

    /**
     * @brief Starts the server and begins accepting client connections.
     * Launches the acceptLoop() in a separate detached thread.
     */
    void start();

    /**
     * @brief Stops the server and closes the listening socket.
     * Terminates the accept loop.
     */
    void stop();

protected:
    // (Reserved for future extensions or inheritance)

private:
    int _sockfd;               ///< File descriptor of the listening TCP socket.
    bool _running;             ///< Flag indicating if the server is currently running.
    uint32_t _nextPlayerId = 1;///< Counter to assign unique player IDs.
    std::thread _acceptThread;  ///< Thread for accepting incoming connections.
    std::vector<std::thread> _clientThread;  ///< Thread for handling client connections.

    /**
     * @brief Main loop to accept incoming client connections.
     * Each new client is handled in a separate thread via handleClient().
     */
    void acceptLoop();

    /**
     * @brief Handles the handshake with a single client.
     * @param clientSock Socket descriptor for the connected client.
     *
     * Responsibilities:
     * - Receive ConnectRequest from the client.
     * - Validate the request type.
     * - Assign playerId and UDP port if valid.
     * - Send ConnectResponse or ErrorResponse back to the client.
     * - Close the client socket when done.
     */
    void handleClient(int clientSock);
};

#endif /* !TCPSERVER_HPP_ */
