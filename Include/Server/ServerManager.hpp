/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ServerManager.hpp
*/

#ifndef SERVERMANAGER_HPP_
#define SERVERMANAGER_HPP_

#include <map>
#include <memory>
#include <atomic>
#include <vector>
#include "Server/Game.hpp"
#include "Server/TCPServer.hpp"
#include "Server/UDPServer.hpp"
#include "Clock.hpp"

/**
 * @file ServerManager.hpp
 * @brief Header file for the ServerManager class.
 */

/**
 * @class ServerManager
 * @brief The central controller for the R-Type server.
 *
 * This class orchestrates the TCP and UDP servers, manages game rooms,
 * and handles the synchronization between network events and game logic.
 * It implements both INetworkHandler (for UDP messages) and ITCPHandler
 * (for TCP lobby actions).
 */
class ServerManager : public INetworkHandler, public ITCPHandler {
public:
    /**
     * @brief Construct a new ServerManager object.
     * Initializes the TCP and UDP servers and the clock.
     */
    ServerManager();

    /**
     * @brief Destroy the ServerManager object.
     * Stops the servers and cleans up resources.
     */
    ~ServerManager();

    /**
     * @brief Starts the server loop.
     * Runs the main loop that keeps the server alive and processes events.
     */
    void run();

    /**
     * @brief Callback for handling received UDP messages.
     * @param data Pointer to the raw data received.
     * @param length Size of the data in bytes.
     * @param clientAddr The source address of the message.
     */
    void onMessageReceived(const char* data, size_t length, const sockaddr_in& clientAddr) override;

    /**
     * @brief Creates a new game room.
     * @return The ID of the newly created room.
     */
    int onCreateRoom() override;

    /**
     * @brief Retrieves a list of all active rooms.
     * @return A vector of RoomSimpleInfo structures.
     */
    std::vector<RoomSimpleInfo> onGetRooms() override;

    /**
     * @brief Handles a player joining a room.
     * @param roomId The ID of the room to join.
     * @param playerId The ID of the player.
     * @param username The username of the player.
     * @return true if the join was successful, false otherwise.
     */
    bool onJoinRoom(int roomId, uint32_t playerId, const std::string& username) override;

    /**
     * @brief Handles a player disconnecting from a room.
     * @param playerId The ID of the disconnected player.
     * @param roomId The ID of the room.
     */
    void onPlayerDisconnect(uint32_t playerId, int roomId) override;

    /**
     * @brief Retrieves the current state of a lobby.
     * @param roomId The ID of the room.
     * @param hostId Reference to store the host ID.
     * @param players Reference to store the list of players.
     */
    void onGetLobbyState(int roomId, uint32_t& hostId, std::vector<std::pair<uint32_t, std::string>>& players) override;

    /**
     * @brief Starts the game in a specific room.
     * @param roomId The ID of the room.
     * @param playerId The ID of the player requesting the start.
     */
    void onStartGame(int roomId, uint32_t playerId) override;

    /**
     * @brief Gets the list of players in a room.
     * @param roomId The ID of the room.
     * @return A vector of player IDs.
     */
    std::vector<uint32_t> onGetPlayersInRoom(int roomId) override;

    /**
     * @brief Checks if the game is starting in a specific room.
     * @param roomId The ID of the room.
     * @return true if the game is starting, false otherwise.
     */
    bool isGameStarting(int roomId) override;

private:
    Clock _clock; /**< Clock for managing game time. */
    std::map<int, std::shared_ptr<Game>> _rooms; /**< Map of active game rooms. */
    TCPServer _tcpServer; /**< The TCP server instance. */
    UDPServer _udpServer; /**< The UDP server instance. */
    std::atomic<bool> _running; /**< Flag indicating if the server manager is running. */
    int _nextRoomId = 0; /**< Counter for assigning unique room IDs. */
    std::mutex _serverMutex; /**< Mutex for thread-safe access to shared resources. */
};

#endif