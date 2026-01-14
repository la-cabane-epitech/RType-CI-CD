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

/**
 * @enum GameStatus
 * @brief Represents the current status of a game room.
 */
enum class GameStatus {
    LOBBY,   ///< The game is in the lobby, waiting for players to join and the host to start.
    PLAYING  ///< The game is currently in progress.
};

/**
 * @class Game
 * @brief Manages the game world, players, and entities.
 */
class Game {
public:
    /**
     * @brief Construct a new Game object.
     * Initializes the game status to LOBBY.
     */
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
     * @brief Creates a charged projectile fired by a player.
     * @param playerId The ID of the shooting player.
     * @param udpServer Reference to the UDP server for spawn notification.
     */
    void createPlayerChargedShot(uint32_t playerId, UDPServer& udpServer);

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
    /**
     * @brief Updates the game level logic, such as enemy spawning patterns over time.
     * @param elapsedTime The time elapsed since the last update.
     */
    void updateGameLevel(float elapsedTime);

    /**
     * @brief Updates the game state (entities, collisions, spawning).
     * @param udpServer Reference to the UDP server.
     */
    void update(UDPServer& udpServer);

    /**
     * @brief Gets the current status of the game (Lobby or Playing).
     * @return The current GameStatus.
     */
    GameStatus getStatus() const;
    /**
     * @brief Sets the status of the game.
     * @param status The new GameStatus to set.
     */
    void setStatus(GameStatus status);
    /**
     * @brief Gets the ID of the host player.
     * The host is typically the first player who joined the room.
     * @return The player ID of the host.
     */
    uint32_t getHostId() const;
    /**
     * @brief Gets a constant reference to the list of players in the game.
     * @return A const reference to the vector of players.
     */
    const std::vector<Player>& getPlayers() const;

private:
    std::vector<Player> _players; /**< List of players in the game. */
    std::mutex _playersMutex; /**< Mutex to protect access to the _players vector. */

    std::vector<Entity> _entities; /**< List of all entities in the game (enemies, projectiles). */
    std::mutex _entitiesMutex; /**< Mutex to protect access to the _entities vector. */
    uint32_t _nextEntityId = 1; /**< Counter for assigning unique entity IDs. */
    /**
     * @brief Checks for AABB collision between two rectangular objects.
     * @param x1 X position of the first object.
     * @param y1 Y position of the first object.
     * @param w1 Width of the first object.
     * @param h1 Height of the first object.
     * @param x2 X position of the second object.
     * @param y2 Y position of the second object.
     * @param w2 Width of the second object.
     * @param h2 Height of the second object.
     * @return true if the objects are colliding, false otherwise.
     */
    bool checkCollision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);
    float _gameTime = 0.0f; /**< Total time the game has been running in the PLAYING state. */
    std::chrono::steady_clock::time_point _lastEnemySpawnTime = std::chrono::steady_clock::now(); /**< Time point of the last enemy spawn. */
    std::chrono::steady_clock::time_point _lastGlobalSyncTime = std::chrono::steady_clock::now(); /**< Time point of the last global state synchronization. */
    static constexpr std::chrono::milliseconds GLOBAL_SYNC_INTERVAL = std::chrono::milliseconds(100); /**< Interval for global state synchronization. */
    GameStatus _status; /**< Current status of the game (Lobby/Playing). */

    void sendGlobalStateSync(UDPServer& udpServer); /**< Sends a global state synchronization packet to all clients. */
};


#endif /* !GAME_HPP_ */
