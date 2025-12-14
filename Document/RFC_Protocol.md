# R-Type Network Protocol Specification (RFC)

## Abstract

This document specifies the communication protocols used in the R-Type project. It defines the packet structures, data types, and interaction flows for both TCP (Control) and UDP (Gameplay) channels.

## 1. Overview

The R-Type multiplayer architecture uses a dual-protocol approach:
1.  **TCP**: Used for session initialization, reliable exchange of critical information (handshake, lobby joining), and assigning unique Player IDs.
2.  **UDP**: Used for the real-time game loop to minimize latency. This includes player movement, entity updates, and game state synchronization.

## 2. Data Types and Conventions

- **Byte Order**: Little-Endian.
- **Structure Packing**: 1-byte alignment (`#pragma pack(1)`).
- **Floating Point**: IEEE 754 32-bit float.
- **Types**:
    - `uint8_t`: Unsigned 8-bit integer.
    - `uint16_t`: Unsigned 16-bit integer.
    - `uint32_t`: Unsigned 32-bit integer.
    - `float`: 32-bit floating point.

## 3. TCP Protocol

The TCP connection is established by the client to the server's listening port.

### 3.1 Handshake & Initialization
Upon successful connection:
1.  **Client** connects to **Server**.
2.  **Server** generates a unique `PlayerId` (uint32_t).
3.  **Server** sends the `PlayerId` to the **Client**.
4.  **Client** stores this ID and uses it for all subsequent UDP `PlayerInputPacket`s.

*(Note: Additional TCP commands for lobby management may be defined in future revisions)*

## 4. UDP Protocol

The UDP protocol handles the high-frequency game state replication.

### 4.1 Packet Header
All UDP packets begin with a 1-byte `type` field identifying the message.

### 4.2 Message Types
Defined in `UDPMessageType`.

| ID | Mnemonic | Direction | Description |
|---|---|---|---|
| 1 | `PLAYER_INPUT` | Client -> Server | Player control inputs |
| 2 | `PLAYER_STATE` | Server -> Client | Authoritative player state |
| 3 | `ENTITY_SPAWN` | Server -> Client | Spawn visual entity |
| 4 | `ENTITY_UPDATE` | Server -> Client | Update entity position |
| 5 | `ENTITY_DESTROY` | Server -> Client | Remove entity |
| 6 | `PING` | Client -> Server | Latency check |
| 7 | `PONG` | Server -> Client | Latency response |
| 8 | `PLAYER_DISCONNECT` | Client -> Server | Graceful disconnect |

### 4.3 Input Bitmask
Defined in `Input` enum. Combined using bitwise OR.

| Bit | Value | Action |
|---|---|---|
| 0 | 0x01 | UP |
| 1 | 0x02 | DOWN |
| 2 | 0x04 | LEFT |
| 3 | 0x08 | RIGHT |
| 4 | 0x10 | SHOOT |

### 4.4 Packet Definitions

#### 4.4.1 Player Input (Type 1)
Sent by the client every tick to inform the server of pressed keys.

```cpp
struct PlayerInputPacket {
    uint8_t type;       // 1
    uint32_t playerId;  // ID received via TCP
    uint32_t tick;      // Client tick counter
    uint8_t inputs;     // Input Bitmask
};
```

#### 4.4.2 Player State (Type 2)
Sent by the server to correct/update the player's position.

```cpp
struct PlayerStatePacket {
    uint8_t type;              // 2
    uint32_t playerId;         // ID of the player being updated
    uint32_t lastProcessedTick;// Last input tick processed by server
    uint32_t timestamp;        // Server timestamp
    float x;                   // X Position
    float y;                   // Y Position
};
```

#### 4.4.3 Entity Spawn (Type 3)
Sent when a new entity (enemy, bullet, powerup) appears.

```cpp
struct EntitySpawnPacket {
    uint8_t type;       // 3
    uint32_t entityId;  // Unique Entity ID
    uint16_t entityType;// Type of entity (sprite/behavior ID)
    uint32_t timestamp; // Server timestamp
    float x;            // Spawn X
    float y;            // Spawn Y
};
```

#### 4.4.4 Entity Update (Type 4)
Sent to update the position of an existing entity.

```cpp
struct EntityUpdatePacket {
    uint8_t type;       // 4
    uint32_t entityId;  // Existing Entity ID
    uint32_t timestamp; // Server timestamp
    float x;            // New X
    float y;            // New Y
};
```

#### 4.4.5 Entity Destroy (Type 5)
Sent when an entity is destroyed or leaves the screen.

```cpp
struct EntityDestroyPacket {
    uint8_t type;       // 5
    uint32_t entityId;  // Entity to remove
};
```

#### 4.4.6 Ping / Pong (Type 6 / 7)
Used to calculate Round Trip Time (RTT).

```cpp
struct PingPacket {
    uint8_t type;       // 6 (Ping) or 7 (Pong)
    uint32_t timestamp; // Originating timestamp
};
```

#### 4.4.7 Player Disconnect (Type 8)
Sent by client before closing the application.

```cpp
struct PlayerDisconnectPacket {
    uint8_t type;       // 8
    uint32_t playerId;  // Player ID
};
```