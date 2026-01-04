/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Renderer.cpp
*/

#include "Client/Ray.hpp"
#include "Client/Renderer.hpp"
#include "Client/ParallaxLayer.hpp"
#include <map>

#include <string>

Renderer::Renderer(GameState& gameState) : _gameState(gameState)
{
    // On charge la texture pour les tirs du joueur (type 1)
    _textures[0] = LoadTexture("Assets/r-typesheet42.gif");
    _textures[1] = LoadTexture("Assets/attack.png");

    _textures[2] = LoadTexture("Assets/enemy.png");
    _textures[10] = LoadTexture("Assets/blue-back.png");
    _textures[11] = LoadTexture("Assets/blue-stars.png");
    _textures[12] = LoadTexture("Assets/asteroid-1.png");

    // Fond : On calcule l'échelle pour remplir l'écran
    float bgScale = (float)GetScreenHeight() / _textures[10].height;
    _parallaxLayers.emplace_back(0.2f, _textures[10], bgScale, 0.0f);

    _parallaxLayers.emplace_back(0.4f, _textures[11], 6.0f, 0.0f);
}

Renderer::~Renderer()
{
    for (auto const& [key, val] : _textures)
    {
        UnloadTexture(val);
    }
}

void Renderer::draw()
{
    float dt = GetFrameTime();
    float scrollSpeed = 150.0f;   // Vitesse de base du scrolling (pixels par seconde)

    for (auto& layer : _parallaxLayers) {
        layer.update(dt, scrollSpeed);
    }

    BeginDrawing();
    ClearBackground(BLACK);

    for (auto& layer : _parallaxLayers) {
        layer.draw();
    }
    DrawText("R-Type", 10, 10, 20, DARKGRAY);

    std::string pingText = "Ping: " + std::to_string(_gameState.rtt) + " ms";
    DrawText(pingText.c_str(), 10, 40, 20, DARKGRAY);

    for (const auto& pair : _gameState.players) {
        Color color = (pair.first == _gameState.myPlayerId) ? BLUE : RED;
        // DrawRectangle(static_cast<int>(pair.second.x - 25), static_cast<int>(pair.second.y - 25), 50, 50, color);
        if (_textures.count(0) && _textures.at(0).id != 0) {
            const Texture2D& texture = _textures.at(0);
            float scale = 2.0f; // On agrandit le vaisseau 3 fois
            Rectangle sourceRec = { 0.0f, 0.0f, 33.0f, 17.0f };
            Rectangle destRec = { pair.second.x, pair.second.y, 33.0f * scale, 17.0f * scale };
            Vector2 origin = { (33.0f * scale) / 2, (17.0f * scale) / 2 }; // Pour centrer l'image agrandie
            DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
        }
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