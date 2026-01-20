/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** GameState.hpp
*/

#ifndef GAMESTATE_HPP_
#define GAMESTATE_HPP_

#include <unordered_map>
#include <cstdint>

/**
 * @struct Position
 * @brief Represents a 2D coordinate in the game world.
 */
struct Position {
    float x; /**< X coordinate */
    float y; /**< Y coordinate */
    float vy = 0.0f; /**< Vertical velocity for animation */
};

/**
 * @struct EntityState
 * @brief Represents the state of a game entity.
 */
struct EntityState {
    float x;       /**< X coordinate */
    float y;       /**< Y coordinate */
    uint16_t type; /**< Type identifier of the entity */
};

/**
 * @struct GameState
 * @brief Holds the synchronized state of the game on the client side.
 *
 * Contains information about players, entities, and network statistics like RTT.
 */
struct GameState {
    uint32_t myPlayerId = 0; /**< The ID of the local player */
    std::unordered_map<uint32_t, Position> players; /**< Map of player IDs to their positions */
    std::unordered_map<uint32_t, EntityState> entities; /**< Map of entity IDs to their states */
    uint32_t rtt = 0; /**< Round Trip Time in milliseconds */
};

#endif