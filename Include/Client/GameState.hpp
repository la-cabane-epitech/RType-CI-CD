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

// struct PlayerDisplay {
//     float x;
//     float y;
// };

// class GameState {
// public:
//     std::unordered_map<uint32_t, PlayerDisplay> players;
//     uint32_t myPlayerId = 0;
// };

// #endif

struct Position {
    float x;
    float y;
};

struct GameState {
    uint32_t myPlayerId = 0;
    std::unordered_map<uint32_t, Position> players;
    std::unordered_map<uint32_t, Position> entities;
    uint32_t rtt = 0;
};

#endif