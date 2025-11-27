/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description: ** ProtocoleUDP
**
** This header defines the UDP protocol structures used for real-time gameplay
** communication between the R-Type client and server.
*/

#ifndef PROTOCOLEUDP_HPP_
    #define PROTOCOLEUDP_HPP_
    #include <cstdint>

// -----------------------------------------
// Ensure structures are packed without padding
// to guarantee identical memory layout across client and server.
// -----------------------------------------
#pragma pack(push, 1)

/**
 * @enum UdpPacketType
 * @brief Defines the type of UDP packet being sent.
 */
enum UdpPacketType : uint8_t {
    CLIENT_INPUT = 10,
    GAME_STATE = 11,
    HEARTBEAT = 12,
};

/**
 * @struct UdpPacketHeader
 * @brief Common header for all UDP packets.
 *
 * Fields:
 * - type: The type of the packet (see UdpPacketType).
 * - sequence: A sequence number to order packets and detect loss.
 */
struct UdpPacketHeader {
    uint8_t type;
    uint16_t sequence;
};

/**
 * @struct ClientInputPayload
 * @brief Sent by the client to inform the server of its actions.
 *
 * Fields:
 * - inputMask: A bitmask representing the player's current inputs.
 */
struct ClientInputPayload {
    uint8_t inputMask; // e.g., 0b00010 for shoot, 0b00001 for up, etc.
};

/**
 * @struct EntityState
 * @brief Represents the state of a single game entity.
 *
 * Fields:
 * - entityId: Unique identifier for the entity.
 * - entityType: Type of the entity (player, enemy, bullet).
 * - x, y: Position of the entity.
 * - vx, vy: Velocity of the entity.
 */
struct EntityState {
    uint32_t entityId;
    uint8_t entityType;
    float x, y;
    float vx, vy;
};

/**
 * @struct GameStatePayload
 * @brief Sent by the server to update clients on the game state.
 *
 * Fields:
 * - tick: The server tick this state corresponds to.
 * - entityCount: Number of entities in this packet.
 * - entities: Array of entity states.
 */
struct GameStatePayload {
    uint32_t tick;
    uint8_t entityCount;
    EntityState entities[32]; // Max 32 entities per packet
};

// You can add more packet structures here as needed, like HeartbeatPayload.

// Restore default packing
#pragma pack(pop)

#endif /* !PROTOCOLEUDP_HPP_ */
