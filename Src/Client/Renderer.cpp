/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Renderer.cpp
*/

#include "Client/Ray.hpp"
#include "Client/Renderer.hpp"
#include <map>
#include <string>

Renderer::Renderer(GameState& gameState) : _gameState(gameState)
{
    // On charge la texture pour les tirs du joueur (type 1)
    _textures[1] = LoadTexture("Assets/attack.png");
}

Renderer::~Renderer()
{
    // On décharge toutes les textures de la map
    for (auto const& [key, val] : _textures)
    {
        UnloadTexture(val);
    }
}

void Renderer::draw()
{

    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("R-Type", 10, 10, 20, DARKGRAY);

    std::string pingText = "Ping: " + std::to_string(_gameState.rtt) + " ms";
    DrawText(pingText.c_str(), 10, 40, 20, DARKGRAY);

    for (const auto& pair : _gameState.players) {
        Color color = (pair.first == _gameState.myPlayerId) ? BLUE : RED;
        DrawRectangle(static_cast<int>(pair.second.x - 25), static_cast<int>(pair.second.y - 25), 50, 50, color);
        DrawText(std::to_string(pair.first).c_str(), 
                static_cast<int>(pair.second.x - 10),
                static_cast<int>(pair.second.y - 10),
                20, 
                WHITE);
    }

    for (const auto& pair : _gameState.entities) {
        const auto& entity = pair.second;

        if (entity.type == 2) {
            DrawRectangle(static_cast<int>(entity.x), static_cast<int>(entity.y), 30, 30, GREEN);
        }
        else if (_textures.count(entity.type)) {
            DrawTexture(_textures.at(entity.type), static_cast<int>(entity.x), static_cast<int>(entity.y), WHITE);
        }
    }

    EndDrawing();
}