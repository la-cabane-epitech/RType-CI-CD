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
    PONG              = 7   // Server → Client
};

/**
 * @struct PlayerInputPacket
 * @brief Sent by the client to inform the server about the player's actions.
 *
 * Fields:
 * - type: Always PLAYER_INPUT
 * - playerId: Player identifier assigned by TCP handshake
 * - tick: Increasing counter used to help server detect late packets
 * - x / y: Directional movement inputs
 * - shooting: Whether the player is firing
 */
struct PlayerInputPacket {
    uint8_t type = PLAYER_INPUT;
    uint32_t playerId;
    uint32_t tick;
    float x;
    float y;
    uint8_t shooting;
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

// Restore packing
#pragma pack(pop)

#endif /* !PROTOCOLEUDP_HPP_ */
