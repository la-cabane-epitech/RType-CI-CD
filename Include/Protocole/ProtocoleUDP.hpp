/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ProtocoleUDP
**
** This header defines the UDP protocol structures used during gameplay
** between the R-Type client and server. All packets are binary and packed
** to ensure identical memory layout across client and server.
*/

#ifndef PROTOCOLEUDP_HPP_
#define PROTOCOLEUDP_HPP_

#include <cstdint>

// -----------------------------------------
// Ensure structures are packed without padding
// -----------------------------------------
#pragma pack(push, 1)

/**
 * @enum Input
 * @brief Defines bitmasks for player actions.
 *
 * Each action is assigned a unique bit to pack multiple inputs into a single byte,
 * saving network bandwidth. The combined state is sent in the `inputs` field
 * of the `PlayerInputPacket`.
 */
enum Input : uint8_t {
    UP = 1 << 0,    ///< Represents bit 0 (value 1)
    DOWN = 1 << 1,  ///< Represents bit 1 (value 2)
    LEFT = 1 << 2,  ///< Represents bit 2 (value 4)
    RIGHT = 1 << 3, ///< Represents bit 3 (value 8)
    SHOOT = 1 << 4  ///< Represents bit 4 (value 16)
};

/**
 * @enum UDPMessageType
 * @brief Identifies the type of each UDP packet sent between client and server.
 */
enum UDPMessageType : uint8_t {
    PLAYER_INPUT      = 1,  // Sent by client → player's commands
    PLAYER_STATE      = 2,  // Sent by server → authoritative player position
    ENTITY_SPAWN      = 3,  // Sent by server → spawn of an entity
    ENTITY_UPDATE     = 4,  // Sent by server → update entity state
    ENTITY_DESTROY    = 5,  // Sent by server → destroy entity
    PING              = 6,  // Client → Server
    PONG              = 7,  // Server → Client
    PLAYER_DISCONNECT = 8   // Sent by client → player is disconnecting
};

/**
 * @struct PlayerInputPacket
 * @brief Sent by the client to inform the server about the player's actions.
 *
 * Fields:
 * - type: Always PLAYER_INPUT
 * - playerId: Player identifier assigned by TCP handshake
 * - tick: Increasing counter used to help server detect late packets
 * - inputs: A bitmask representing all player actions (up, down, left, right, shoot).
 */
struct PlayerInputPacket {
    uint8_t type = PLAYER_INPUT;
    uint32_t playerId;
    uint32_t tick;
    uint8_t inputs; // Bit 0: UP, Bit 1: DOWN, Bit 2: LEFT, Bit 3: RIGHT, Bit 4: SHOOT
};

/**
 * @struct PlayerStatePacket
 * @brief Sent by the server to update the player's authoritative state.
 *
 * Fields:
 * - type: PLAYER_STATE
 * - playerId: Player concerned
 * - x / y: Authoritative position on the map
 */
struct PlayerStatePacket {
    uint8_t type = PLAYER_STATE;
    uint32_t playerId;
    float x;
    float y;
};

/**
 * @struct EntitySpawnPacket
 * @brief Sent by the server to spawn a new enemy, projectile, etc.
 *
 * Fields:
 * - type: ENTITY_SPAWN
 * - entityId: Unique ID of the entity
 * - entityType: Defines what type of entity (enemy, bullet, etc.)
 * - x / y: Spawn coordinates
 */
struct EntitySpawnPacket {
    uint8_t type = ENTITY_SPAWN;
    uint32_t entityId;
    uint16_t entityType;
    float x;
    float y;
};

/**
 * @struct EntityUpdatePacket
 * @brief Sent by the server to update an entity’s movement/state.
 *
 * Fields:
 * - type: ENTITY_UPDATE
 * - entityId: ID of the entity to update
 * - x / y: New authoritative position
 */
struct EntityUpdatePacket {
    uint8_t type = ENTITY_UPDATE;
    uint32_t entityId;
    float x;
    float y;
};

/**
 * @struct EntityDestroyPacket
 * @brief Informs the client that an entity should be removed.
 *
 * Fields:
 * - type: ENTITY_DESTROY
 * - entityId: Entity to destroy
 */
struct EntityDestroyPacket {
    uint8_t type = ENTITY_DESTROY;
    uint32_t entityId;
};

/**
 * @struct PingPacket
 * @brief Used to measure latency.
 */
struct PingPacket {
    uint8_t type = PING;
    uint32_t timestamp; // client clock
};

/**
 * @struct PongPacket
 * @brief Server reply to PingPacket.
 */
struct PongPacket {
    uint8_t type = PONG;
    uint32_t timestamp;
};

/**
 * @struct PlayerDisconnectPacket
 * @brief Sent by the client to inform the server it is disconnecting.
 *
 * Fields:
 * - type: Always PLAYER_DISCONNECT
 * - playerId: Player identifier
 */
struct PlayerDisconnectPacket {
    uint8_t type = PLAYER_DISCONNECT;
    uint32_t playerId;
};

// Restore packing
#pragma pack(pop)

#endif /* !PROTOCOLEUDP_HPP_ */
