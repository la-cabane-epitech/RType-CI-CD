/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ProtocoleTCP
**
** This header defines the TCP protocol structures used for the initial handshake
** between the R-Type client and server. The protocol allows clients to connect,
** receive a unique player ID and UDP port for gameplay, and receive error messages.
*/

#ifndef PROTOCOLETCP_HPP_
#define PROTOCOLETCP_HPP_

#include <cstdint>
#include "Clock.hpp"

/**
 * @file ProtocoleTCP.hpp
 * @brief TCP protocol definitions for the R-Type game.
 */

// -----------------------------------------
// Ensure structures are packed without padding
// to guarantee identical memory layout across client and server.
// -----------------------------------------
#pragma pack(push, 1)

/**
 * @enum TCPMessageType
 * @brief Identifies the type of each TCP packet.
 */
enum TCPMessageType : uint8_t {
    CONNECT = 1,
    CONNECT_OK = 2,
    CONNECT_ERROR = 3,
    LIST_ROOMS = 4,
    LIST_ROOMS_RESPONSE = 5,
    CREATE_ROOM = 6,
    CREATE_ROOM_RESPONSE = 7,
    JOIN_ROOM = 8,
    JOIN_ROOM_RESPONSE = 9,
    GET_LOBBY_STATE = 10,
    LOBBY_STATE_RESPONSE = 11,
    START_GAME_REQUEST = 12,
    GAME_STARTING_NOTIFICATION = 13,
    CHAT_MESSAGE = 20
};

/**
 * @struct ConnectRequest
 * @brief Sent by the client to request a connection to the server.
 *
 * Fields:
 * - type: Packet type identifier
 * - username: Null-terminated username of the player (max 31 chars)
 */
struct ConnectRequest {
    uint8_t type = TCPMessageType::CONNECT;     ///< Packet type identifier
    char username[32];    ///< Player username
};

/**
 * @struct ConnectResponse
 * @brief Sent by the server to confirm a successful connection.
 *
 * Fields:
 * - type: Packet type identifier
 * - playerId: Unique identifier assigned to the player
 * - udpPort: UDP port number assigned for gameplay communication
 */
struct ConnectResponse {
    uint8_t type = TCPMessageType::CONNECT_OK;     ///< Packet type identifier
    uint32_t playerId;    ///< Unique player ID
    uint16_t udpPort;     ///< Assigned UDP port for gameplay
    Clock clock;          ///< Server clock snapshot for synchronization
};

/**
 * @struct ErrorResponse
 * @brief Sent by the server if the connection request fails or is invalid.
 *
 * Fields:
 * - type: Packet type identifier
 * - message: Null-terminated error message (max 63 characters)
 */
struct ErrorResponse {
    uint8_t type = TCPMessageType::CONNECT_ERROR;     ///< Packet type identifier
    char message[64];     ///< Error message text
};

/**
 * @struct RoomInfo
 * @brief Information about a game room.
 */
struct RoomInfo {
    int32_t id;           ///< Unique identifier for the room.
    int32_t playerCount;  ///< Current number of players in the room.
    int32_t maxPlayers;   ///< Maximum number of players allowed in the room.
};

/**
 * @struct ListRoomsRequest
 * @brief Sent by a client to request the list of available game rooms.
 */
struct ListRoomsRequest {
    uint8_t type = TCPMessageType::LIST_ROOMS; ///< Packet type identifier.
};

/**
 * @struct ListRoomsResponse
 * @brief Sent by the server in response to a ListRoomsRequest.
 *
 * This packet is followed by `count` number of `RoomInfo` structs.
 */
struct ListRoomsResponse {
    uint8_t type = TCPMessageType::LIST_ROOMS_RESPONSE; ///< Packet type identifier.
    int32_t count; ///< Number of rooms available.
};

/**
 * @struct CreateRoomRequest
 * @brief Sent by a client to request the creation of a new game room.
 */
struct CreateRoomRequest {
    uint8_t type = TCPMessageType::CREATE_ROOM; ///< Packet type identifier.
};

/**
 * @struct CreateRoomResponse
 * @brief Sent by the server to confirm room creation.
 */
struct CreateRoomResponse {
    uint8_t type = TCPMessageType::CREATE_ROOM_RESPONSE; ///< Packet type identifier.
    int32_t roomId; ///< The ID of the newly created room. -1 on failure.
};

/**
 * @struct JoinRoomRequest
 * @brief Sent by a client to join an existing game room.
 */
struct JoinRoomRequest {
    uint8_t type = TCPMessageType::JOIN_ROOM; ///< Packet type identifier.
    int32_t roomId; ///< The ID of the room to join.
};

/**
 * @struct JoinRoomResponse
 * @brief Sent by the server to confirm if a player successfully joined a room.
 */
struct JoinRoomResponse {
    uint8_t type = TCPMessageType::JOIN_ROOM_RESPONSE; ///< Packet type identifier.
    int32_t status; ///< Status of the join request (e.g., 0 for success, -1 for full, -2 for not found).
};

/**
 * @struct LobbyPlayerInfo
 * @brief Contains basic information about a player in a lobby.
 */
struct LobbyPlayerInfo {
    uint32_t playerId;  ///< The unique ID of the player.
    char username[32];  ///< The username of the player.
};

/**
 * @struct GetLobbyStateRequest
 * @brief Sent by a client to get the current state of their lobby.
 */
struct GetLobbyStateRequest {
    uint8_t type = TCPMessageType::GET_LOBBY_STATE; ///< Packet type identifier.
};

/**
 * @struct LobbyStateResponse
 * @brief Sent by the server with the current state of the lobby.
 *
 * This packet is followed by `playerCount` number of `LobbyPlayerInfo` structs.
 */
struct LobbyStateResponse {
    uint8_t type = TCPMessageType::LOBBY_STATE_RESPONSE; ///< Packet type identifier.
    uint32_t hostId; ///< The ID of the player who is the host.
    int32_t playerCount; ///< The number of players in the lobby.
    // Followed by 'playerCount' LobbyPlayerInfo structs
};

/**
 * @struct StartGameRequest
 * @brief Sent by the host client to start the game.
 */
struct StartGameRequest {
    uint8_t type = TCPMessageType::START_GAME_REQUEST; ///< Packet type identifier.
};

/**
 * @struct GameStartingNotification
 * @brief Broadcast by the server to all clients in a lobby when the game is starting.
 */
struct GameStartingNotification {
    uint8_t type = TCPMessageType::GAME_STARTING_NOTIFICATION; ///< Packet type identifier.
};


// Restore default packing
#pragma pack(pop)

#endif /* !PROTOCOLETCP_HPP_ */
