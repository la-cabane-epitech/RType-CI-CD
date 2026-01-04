/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Renderer.hpp
*/

#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "GameState.hpp"
#include "ParallaxLayer.hpp"
#include <map>

class Renderer {
public:
    Renderer(GameState& gameState);
    ~Renderer();
    void draw();

private:
    GameState& _gameState;
    std::map<uint16_t, Texture2D> _textures;
    std::vector<ParallaxLayer> _parallaxLayers;
};

#endif