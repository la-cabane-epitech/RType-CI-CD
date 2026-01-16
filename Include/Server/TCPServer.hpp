/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPServer.hpp
*/

#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#include <thread>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include "Client/Asio.hpp"
#include "ITCPHandler.hpp"
#include "Protocole/ProtocoleTCP.hpp"
#include "Clock.hpp"

/**
 * @file TCPServer.hpp
 * @brief Header file for the TCPServer class.
 */

/**
 * @class TCPServer
 * @brief Manages the TCP server for handling client connections and game lobby logic.
 *
 * The TCPServer class is responsible for accepting incoming TCP connections,
 * handling the initial handshake, and managing client requests within the lobby
 * (listing rooms, creating rooms, joining rooms, etc.). It delegates specific
 * game logic actions to an ITCPHandler implementation.
 */
class TCPServer {
public:
    /**
     * @brief Construct a new TCPServer object.
     * @param port The port number to listen on.
     * @param handler Pointer to the handler for game logic events.
     * @param clock Reference to the shared Clock object.
     */
    TCPServer(int port, ITCPHandler* handler, Clock& clock);

    /**
     * @brief Destroy the TCPServer object.
     * Stops the server and joins all threads.
     */
    ~TCPServer();

    /**
     * @brief Starts the TCP server.
     * Initializes the acceptor and starts the thread for accepting connections.
     */
    void start();

    /**
     * @brief Stops the TCP server.
     * Closes the acceptor and all client sockets, and stops all threads.
     */
    void stop();

    /**
     * @brief Sends a notification to all players in a room that the game is starting.
     * @param roomId The ID of the room where the game is starting.
     */
    void sendGameStartingNotification(int roomId);

private:
    /**
     * @brief Main loop for accepting new client connections.
     */
    void acceptLoop();

    /**
     * @brief Handles the communication with a connected client in the lobby.
     * @param clientSocket Shared pointer to the client's socket.
     */
    void handleClient(std::shared_ptr<asio::ip::tcp::socket> clientSocket);

    /**
     * @brief Handles the communication with a client that has joined a room.
     * @param clientSocket Shared pointer to the client's socket.
     * @param roomId The ID of the room the client is in.
     * @param playerId The ID of the client.
     */
    void handleInRoomClient(std::shared_ptr<asio::ip::tcp::socket> clientSocket, int roomId, uint32_t playerId);

    asio::io_context _io_context; /**< ASIO IO context for managing I/O operations. */
    asio::ip::tcp::acceptor _acceptor; /**< TCP acceptor for listening to incoming connections. */

    bool _running; /**< Flag indicating if the server is running. */

    ITCPHandler* _handler; /**< Pointer to the handler for game logic events. */

    uint32_t _nextPlayerId = 1; /**< Counter for assigning unique player IDs. */
    int _nextRoomId = 0; /**< Counter for assigning unique room IDs. */
    std::map<uint32_t, std::string> _playerUsernames; /**< Map of player IDs to usernames. */
    std::mutex _serverMutex; /**< Mutex for thread safety. */
    std::map<uint32_t, std::shared_ptr<asio::ip::tcp::socket>> _playerSockets; /**< Map of player IDs to their sockets. */
    std::map<uint32_t, int> _playerRoomMap; /**< Map of player IDs to the room ID they are currently in. */

    std::thread _acceptThread; /**< Thread for the accept loop. */
    std::vector<std::thread> _clientThread; /**< Vector of threads handling individual clients. */
    const Clock& _clock; /**< Reference to the shared Clock object. */
};

#endif /* !TCPSERVER_HPP_ */