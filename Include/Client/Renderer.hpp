/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Renderer.hpp
*/

#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "GameState.hpp"
#include <map>
#include <vector>
#include "Client/Ray.hpp"

struct Star {
    float x;
    float y;
    float speed;
    float scale;
};

class Renderer {
public:
    Renderer(GameState& gameState);
    ~Renderer();
    void draw();

private:
    GameState& _gameState;
    std::map<uint16_t, Texture2D> _textures;
    std::vector<Star> _stars;
    Texture2D _starTexture;
};

#endif