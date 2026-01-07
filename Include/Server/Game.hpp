/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Game.hpp
*/

#ifndef GAME_HPP_
#define GAME_HPP_

#include <cstdint>
#include <vector>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <chrono>

#include "CrossPlatformSocket.hpp"
#include "Protocole/ProtocoleUDP.hpp"
class UDPServer;

/**
 * @file Game.hpp
 * @brief Server-side game logic and state management.
 */

/**
 * @struct Player
 * @brief Represents a player in the game session.
 */
struct Player {
    uint32_t id;                     ///< Unique player identifier
    char username[32];               ///< Player username
    float x = 400;                   ///< X position
    float y = 225;                   ///< Y position
    float velocity = 5;              ///< Movement speed
    sockaddr_in udpAddr;             ///< Player's UDP address for updates
    uint32_t lastProcessedTick = 0;  ///< Last input tick processed
    bool addrSet = false;            ///< Whether the UDP address has been resolved
    int height = 32;                 ///< Hitbox height
    int width = 32;                  ///< Hitbox width
};

/**
 * @struct Entity
 * @brief Represents a game entity (enemy, projectile, etc.).
 */
struct Entity {
    uint32_t id;             ///< Unique entity identifier
    uint16_t type;           ///< Entity type
    float x;                 ///< X position
    float y;                 ///< Y position
    float velocityX = 10.0f; ///< Horizontal velocity
    float velocityY = 0.0f;  ///< Vertical velocity
    int height = 0;          ///< Hitbox height
    int width = 0;           ///< Hitbox width
    bool is_collide = false; ///< Collision state
};

enum class GameStatus {
    LOBBY,
    PLAYING
};

/**
 * @class Game
 * @brief Manages the game world, players, and entities.
 */
class Game {
public:
    Game() : _status(GameStatus::LOBBY) {}

    /**
     * @brief Adds a new player to the game.
     * @param playerId The unique ID of the player.
     */
    void addPlayer(uint32_t playerId, const char* username);

    /**
     * @brief Updates the UDP address associated with a player.
     * @param playerId The player's ID.
     * @param udpAddr The new UDP address.
     */
    void updatePlayerUdpAddr(uint32_t playerId, const sockaddr_in& udpAddr);

    /**
     * @brief Retrieves a player by ID.
     * @param playerId The player's ID.
     * @return Pointer to the Player struct, or nullptr if not found.
     */
    Player* getPlayer(uint32_t playerId);

    /**
     * @brief Broadcasts the current game state to all connected players via UDP.
     * @param udpServer Reference to the UDP server instance.
     */
    void broadcastGameState(UDPServer& udpServer);

    /**
     * @brief Updates the last processed input tick for a player.
     * @param playerId The player's ID.
     * @param tick The tick number.
     */
    void setPlayerLastProcessedTick(uint32_t playerId, uint32_t tick);

    /**
     * @brief Creates a projectile fired by a player.
     * @param playerId The ID of the shooting player.
     * @param udpServer Reference to the UDP server for spawn notification.
     */
    void createPlayerShot(uint32_t playerId, UDPServer& udpServer);

    /**
     * @brief Updates positions and states of all entities.
     * @param udpServer Reference to the UDP server for updates.
     */
    void updateEntities(UDPServer& udpServer);

    /**
     * @brief Spawns a new enemy entity.
     * @param udpServer Reference to the UDP server for spawn notification.
     */
    void createEnemy(UDPServer& udpServer);

    /**
     * @brief Removes a player from the game.
     * @param playerId The ID of the player to disconnect.
     * @param udpServer Reference to the UDP server for notification.
     */
    void disconnectPlayer(uint32_t playerId, UDPServer& udpServer);

    /**
     * @brief Removes a player from the lobby (without UDP notification).
     * @param playerId The ID of the player to remove.
     */
    void removePlayerFromLobby(uint32_t playerId);

    /**
     * @brief Gets the current number of players in the game.
     * @return The number of players.
     */
    int getPlayerCount();

    /**
     * @brief Checks and resolves collisions between entities and players.
     */
    void handleCollision();
    void updateGameLevel(float elapsedTime);

    /**
     * @brief Updates the game state (entities, collisions, spawning).
     * @param udpServer Reference to the UDP server.
     */
    void update(UDPServer& udpServer);

    GameStatus getStatus() const;
    void setStatus(GameStatus status);
    uint32_t getHostId() const;
    const std::vector<Player>& getPlayers() const;

private:
    std::vector<Player> _players;
    std::mutex _playersMutex;

    std::vector<Entity> _entities;
    std::mutex _entitiesMutex;
    uint32_t _nextEntityId = 1;
    bool checkCollision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);
    float _gameTime = 0.0f;
    std::chrono::steady_clock::time_point _lastEnemySpawnTime = std::chrono::steady_clock::now();
    GameStatus _status;
};


#endif /* !GAME_HPP_ */
