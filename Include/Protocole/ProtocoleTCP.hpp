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

// -----------------------------------------
// Ensure structures are packed without padding
// to guarantee identical memory layout across client and server.
// -----------------------------------------
#pragma pack(push, 1)

/**
 * @struct ConnectRequest
 * @brief Sent by the client to request a connection to the server.
 *
 * Fields:
 * - type: Packet type identifier (1 = CONNECT)
 * - username: Null-terminated username of the player (max 31 chars)
 */
struct ConnectRequest {
    uint8_t type = 1;     // CONNECT
    char username[32];    // Player username
};

/**
 * @struct ConnectResponse
 * @brief Sent by the server to confirm a successful connection.
 *
 * Fields:
 * - type: Packet type identifier (2 = CONNECT_OK)
 * - playerId: Unique identifier assigned to the player
 * - udpPort: UDP port number assigned for gameplay communication
 */
struct ConnectResponse {
    uint8_t type = 2;     // CONNECT_OK
    uint32_t playerId;    // Unique player ID
    uint16_t udpPort;     // Assigned UDP port for gameplay
};

/**
 * @struct ErrorResponse
 * @brief Sent by the server if the connection request fails or is invalid.
 *
 * Fields:
 * - type: Packet type identifier (3 = CONNECT_ERROR)
 * - message: Null-terminated error message (max 63 characters)
 */
struct ErrorResponse {
    uint8_t type = 3;     // CONNECT_ERROR
    char message[64];     // Error message text
};

// Restore default packing
#pragma pack(pop)

#endif /* !PROTOCOLETCP_HPP_ */
