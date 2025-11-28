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

struct PlayerDisplay {
    float x;
    float y;
};

class GameState {
public:
    std::unordered_map<uint32_t, PlayerDisplay> players;
    uint32_t myPlayerId = 0;
};

#endif