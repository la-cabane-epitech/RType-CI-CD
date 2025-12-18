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

/**
 * @class Renderer
 * @brief Responsible for rendering the game state to the screen.
 *
 * Uses Raylib to draw players and entities based on the current GameState.
 */
class Renderer {
public:
    /**
     * @brief Construct a new Renderer object.
     * @param gameState Reference to the shared GameState object.
     */
    Renderer(GameState& gameState);

    /**
     * @brief Destroy the Renderer object.
     */
    ~Renderer();

    /**
     * @brief Draws the current frame based on the game state.
     */
    void draw();

private:
    GameState& _gameState; /**< Reference to the game state to render */
    std::map<uint16_t, Texture2D> _textures; /**< Cache of textures indexed by entity type */
    std::vector<Star> _stars; /**< List of star entities for the background animation */
    Texture2D _starTexture; /**< Texture resource for the star sprite */
};

#endif