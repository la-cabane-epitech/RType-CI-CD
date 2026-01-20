/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ITCPHandler
*/

#ifndef NETWORK_ITCPHANDLER_HPP_
#define NETWORK_ITCPHANDLER_HPP_

#include <string>
#include <vector>
#include <cstdint>

/**
 * @file ITCPHandler.hpp
 * @brief Interface for handling TCP-related game logic events.
 */

namespace Network {

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

    virtual bool onJoinRoom(int roomId, uint32_t playerId, const std::string& username) = 0;

    virtual void onGetLobbyState(int roomId, uint32_t& hostId, std::vector<std::pair<uint32_t, std::string>>& players) = 0;

    virtual void onStartGame(int roomId, uint32_t playerId) = 0;

    virtual void onPlayerDisconnect(uint32_t playerId, int roomId) = 0;

    virtual std::vector<RoomSimpleInfo> onGetRooms() = 0;

    virtual std::vector<uint32_t> onGetPlayersInRoom(int roomId) = 0;

    virtual bool isGameStarting(int roomId) = 0;
};

}

#endif /* !NETWORK_ITCPHANDLER_HPP_ */