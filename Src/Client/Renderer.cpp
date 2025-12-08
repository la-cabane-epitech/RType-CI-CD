/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Renderer.cpp
*/

#include "Client/Ray.hpp"
#include "Client/Renderer.hpp"
#include <string>

Renderer::Renderer(GameState& gameState) : _gameState(gameState)
{
    _playerSpritesheet = LoadTexture("Src/assets/spaceship_sheet.png");
}

Renderer::~Renderer()
{
    // On libère la texture de la mémoire quand le Renderer est détruit.
    UnloadTexture(_playerSpritesheet);
}

void Renderer::draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("R-Type", 10, 10, 20, DARKGRAY);

    // Index du sprite à afficher (0, 1, ou 2)
    int spriteIndex = 1;

    const float SPRITE_WIDTH = 33.0f;
    const float SPRITE_HEIGHT = 17.0f;
    float sourceX = (float)spriteIndex * SPRITE_WIDTH;

    for (const auto& pair : _gameState.players) {
        const auto& player = pair.second;

        Rectangle sourceRec = { sourceX, 0.0f, SPRITE_WIDTH, SPRITE_HEIGHT };
        Vector2 position = { player.x, player.y };

        DrawTextureRec(_playerSpritesheet, sourceRec, position, WHITE);
    }

    EndDrawing();
}