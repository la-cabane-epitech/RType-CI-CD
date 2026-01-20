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

/**
 * @file ProtocoleUDP.hpp
 * @brief UDP protocol definitions for the R-Type game.
 */

// -----------------------------------------
// Ensure structures are packed without padding
// -----------------------------------------
#pragma pack(push, 1)

static constexpr size_t MAX_UDP_PACKET_SIZE = 1024; // Maximum size for UDP packets

/**
 * @enum Input
 * @brief Defines bitmasks for player actions.
 *
 * Each action is assigned a unique bit to pack multiple inputs into a single byte,
 * saving network bandwidth. The combined state is sent in the `inputs` field
 * of the `PlayerInputPacket`.
 */
enum Input : uint8_t {
    UP = 1 << 0,    ///< Move Up (Bit 0)
    DOWN = 1 << 1,  ///< Move Down (Bit 1)
    LEFT = 1 << 2,  ///< Move Left (Bit 2)
    RIGHT = 1 << 3, ///< Move Right (Bit 3)
    PRESSED = 1 << 4,  ///< Shoot (Bit 4)
    HOLD = 1 << 5
};

/**
 * @enum UDPMessageType
 * @brief Identifies the type of each UDP packet sent between client and server.
 */
enum UDPMessageType : uint8_t {
    PLAYER_INPUT      = 1,  ///< Sent by client: player's commands
    PLAYER_STATE      = 2,  ///< Sent by server: authoritative player position
    ENTITY_SPAWN      = 3,  ///< Sent by server: spawn of an entity
    ENTITY_UPDATE     = 4,  ///< Sent by server: update entity state
    ENTITY_DESTROY    = 5,  ///< Sent by server: destroy entity
    PING              = 6,  ///< Client to Server ping
    PONG              = 7,  ///< Server to Client pong
    PLAYER_DISCONNECT = 8,  ///< Sent by client: player is disconnecting
    GLOBAL_STATE_SYNC = 9,   ///< Sent by server: full game state synchrnization
    YOU_HAVE_BEEN_KICKED = 10
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
    uint8_t type = PLAYER_INPUT; ///< Packet type (PLAYER_INPUT)
    uint32_t playerId;           ///< Player identifier
    uint32_t tick;               ///< Input tick counter
    uint8_t inputs;              ///< Bitmask of actions (Input enum)
};

/**
 * @struct PlayerStatePacket
 * @brief Sent by the server to update the player's authoritative state.
 *
 * Fields:
 * - type: PLAYER_STATE.
 * - sequence: A sequence number that increments with each packet, used for packet loss detection.
 * - playerId: ID of the player this state belongs to.
 * - lastProcessedTick: The last input tick from the client that the server has processed.
 * - timestamp: Server time when the state was generated, for interpolation.
 * - x / y: Authoritative position on the map.
 */
struct PlayerStatePacket {
    uint8_t type = PLAYER_STATE; ///< Packet type (PLAYER_STATE)
    uint32_t sequence;           ///< Packet sequence number for loss detection
    uint32_t playerId;           ///< Player identifier
    uint32_t lastProcessedTick;  ///< Last input tick processed by server
    uint32_t timestamp;          ///< Server timestamp
    float x;                     ///< X position
    float y;                     ///< Y position
};

/**
 * @struct EntitySpawnPacket
 * @brief Sent by the server to spawn a new entity (enemy, projectile, etc.).
 *
 * Fields:
 * - type: ENTITY_SPAWN
 * - entityId: Unique ID of the entity
 * - timestamp: Server time when the entity was spawned
 * - entityType: Defines what type of entity (enemy, bullet, etc.)
 * - x / y: Spawn coordinates
 */
struct EntitySpawnPacket {
    uint8_t type = ENTITY_SPAWN; ///< Packet type (ENTITY_SPAWN)
    uint32_t entityId;           ///< Unique entity ID
    uint16_t entityType;         ///< Type of entity
    uint32_t timestamp;          ///< Server timestamp
    float x;                     ///< Spawn X position
    float y;                     ///< Spawn Y position
};

/**
 * @struct EntityUpdatePacket
 * @brief Sent by the server to update an entity’s movement/state.
 *
 * Fields:
 * - type: ENTITY_UPDATE
 * - entityId: ID of the entity to update
 * - timestamp: Server time when the state was generated, for interpolation
 * - x / y: New authoritative position
 */
struct EntityUpdatePacket {
    uint8_t type = ENTITY_UPDATE; ///< Packet type (ENTITY_UPDATE)
    uint32_t entityId;            ///< Entity ID
    uint32_t timestamp;           ///< Server timestamp
    float x;                      ///< New X position
    float y;                      ///< New Y position
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
    uint8_t type = ENTITY_DESTROY; ///< Packet type (ENTITY_DESTROY)
    uint32_t entityId;             ///< Entity ID to destroy
};

/**
 * @struct PingPacket
 * @brief Used to measure latency.
 */
struct PingPacket {
    uint8_t type = PING; ///< Packet type (PING)
    uint32_t timestamp;  ///< Client timestamp
};

/**
 * @struct PongPacket
 * @brief Server reply to PingPacket.
 */
struct PongPacket {
    uint8_t type = PONG; ///< Packet type (PONG)
    uint32_t timestamp;  ///< Original client timestamp
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
    uint8_t type = PLAYER_DISCONNECT; ///< Packet type (PLAYER_DISCONNECT)
    uint32_t playerId;                ///< Player identifier
};

/**
 * @struct SyncedEntityState
 * @brief Represents the minimal state of an entity for global synchronization.
 *
 * Fields:
 * - entityId: Unique ID of the entity
 * - entityType: Defines what type of entity (enemy, bullet, etc.)
 * - x / y: Current coordinates
 */
struct SyncedEntityState {
    uint32_t entityId;           ///< Unique entity ID
    uint16_t entityType;         ///< Type of entity
    float x;                     ///< X position
    float y;                     ///< Y position
};

/**
 * @struct GlobalStateSyncPacket
 * @brief Sent by the server to synchronize the entire game state.
 * The actual entity data (SyncedEntityState) follows this header.
 */
struct GlobalStateSyncPacket {
    uint8_t type = GLOBAL_STATE_SYNC; ///< Packet type (GLOBAL_STATE_SYNC)
    uint32_t entityCount;              ///< Number of entities included in this packet
};

/**
 * @struct YouHaveBeenKickedPacket
 * @brief Sent by the server to a player who has been kicked from a room.
 */
struct YouHaveBeenKickedPacket {
    uint8_t type = YOU_HAVE_BEEN_KICKED; ///< Packet type (YOU_HAVE_BEEN_KICKED)
};


// Restore packing
#pragma pack(pop)

#endif /* !PROTOCOLEUDP_HPP_ */
