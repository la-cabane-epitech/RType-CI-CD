#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>


#include <thread>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
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
     * @param rooms Reference to the map of rooms.
     * @param clock Reference to the shared Clock.
     */
    TCPServer(int port, std::map<int, std::shared_ptr<Game>>& rooms, Clock& clock);

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

    /**
     * @brief Creates a new game room.
     * @return The ID of the newly created room.
     */
    int createRoom();

    /**
     * @brief Handles a client that is already in a room.
     * This loop manages lobby-specific actions like starting the game.
     * @param clientSock The client's socket file descriptor.
     * @param roomId The ID of the room the client is in.
     * @param playerId The ID of the player.
     */
    void handleInRoomClient(int clientSock, int roomId, uint32_t playerId);

private:
    /**
     * @brief The main loop for accepting new client connections.
     * Spawns a new thread to handle each client.
     */
    void acceptLoop();
    /**
     * @brief Handles a new client connection before they join a room.
     * This involves the initial handshake, and room selection/creation.
     * @param clientSock The client's socket file descriptor.
     */
    void handleClient(int clientSock);

    int _sockfd; /**< Server socket file descriptor */
    bool _running; /**< Running state flag */
    std::map<int, std::shared_ptr<Game>>& _rooms; /**< Reference to the rooms */
    uint32_t _nextPlayerId = 1; /**< Counter for assigning unique player IDs */
    int _nextRoomId = 0; /**< Counter for assigning unique room IDs. */
    std::map<uint32_t, std::string> _playerUsernames; /**< Map of player IDs to their usernames. */
    std::mutex _serverMutex; /**< Mutex to protect shared server resources like player/room counters. */

    std::thread _acceptThread; /**< Thread for accepting new connections */
    std::vector<std::thread> _clientThread; /**< Threads for handling individual clients. */
    const Clock& _clock; /**< Reference to the server clock */
};

#endif
