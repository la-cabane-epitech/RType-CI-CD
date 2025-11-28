/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Renderer.hpp
*/

#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "GameState.hpp"

class Renderer {
public:
    Renderer(GameState& gameState);
    void draw();

private:
    GameState& _gameState;
};

#endif