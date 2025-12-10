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
    _attack = LoadTexture("Assets/attack.png");
}

Renderer::~Renderer()
{
    UnloadTexture(_attack);
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
                static_cast<int>(pair.second.x - 10), // Cast explicite ici
                static_cast<int>(pair.second.y - 10), // Et ici
                20, 
                WHITE);
    }

    for (const auto& pair : _gameState.entities) {
        DrawTexture(_attack, static_cast<int>(pair.second.x), static_cast<int>(pair.second.y), WHITE);
    }

    EndDrawing();
}