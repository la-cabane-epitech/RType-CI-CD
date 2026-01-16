/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ITCPHandler
*/

#ifndef ITCPHANDLER_HPP_
#define ITCPHANDLER_HPP_

#include <string>
#include <vector>

/**
 * @file ITCPHandler.hpp
 * @brief Interface for handling TCP-related game logic events.
 */

/**
 * @struct RoomSimpleInfo
 * @brief Simplified structure containing basic information about a room.
 *
 * Used for listing available rooms to clients.
 */
struct RoomSimpleInfo {
    int id;          /**< The unique identifier of the room. */
    int playerCount; /**< The current number of players in the room. */
};

/**
 * @class ITCPHandler
 * @brief Interface to delegate Lobby logic outside of the network library.
 *
 * This interface allows the TCPServer to trigger game logic actions (like creating rooms,
 * joining players, starting games) without knowing the implementation details of the
 * ServerManager or Game classes.
 */
class ITCPHandler {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~ITCPHandler() = default;
    
    /**
     * @brief Handles a request to create a new room.
     * @return The ID of the newly created room.
     */
    virtual int onCreateRoom() = 0;

    /**
     * @brief Handles a request for a player to join a specific room.
     * @param roomId The ID of the room to join.
     * @param playerId The ID of the player attempting to join.
     * @param username The username of the player.
     * @return true if the player successfully joined, false otherwise (e.g., room full).
     */
    virtual bool onJoinRoom(int roomId, uint32_t playerId, const std::string& username) = 0;

    /**
     * @brief Retrieves the current state of a lobby.
     * @param roomId The ID of the room.
     * @param hostId Reference to store the ID of the host player.
     * @param players Reference to a vector to store the list of players (ID and Username).
     */
    virtual void onGetLobbyState(int roomId, uint32_t& hostId, std::vector<std::pair<uint32_t, std::string>>& players) = 0;

    /**
     * @brief Handles a request to start the game in a specific room.
     * @param roomId The ID of the room.
     * @param playerId The ID of the player requesting the start (usually must be host).
     */
    virtual void onStartGame(int roomId, uint32_t playerId) = 0;

    /**
     * @brief Handles the disconnection of a player from a room.
     * @param playerId The ID of the disconnected player.
     * @param roomId The ID of the room the player was in.
     */
    virtual void onPlayerDisconnect(uint32_t playerId, int roomId) = 0;

    /**
     * @brief Retrieves a list of all available rooms.
     * @return A vector of RoomSimpleInfo structures describing the rooms.
     */
    virtual std::vector<RoomSimpleInfo> onGetRooms() = 0;

    /**
     * @brief Retrieves the IDs of all players currently in a specific room.
     * @param roomId The ID of the room.
     * @return A vector of player IDs.
     */
    virtual std::vector<uint32_t> onGetPlayersInRoom(int roomId) = 0;

    /**
     * @brief Checks if the game in a specific room is currently starting.
     * @param roomId The ID of the room.
     * @return true if the game is starting, false otherwise.
     */
    virtual bool isGameStarting(int roomId) = 0;
};

#endif /* !ITCPHANDLER_HPP_ */