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
#include <vector>

struct EntityRenderConfig {
    int textureId;      // ID de la texture chargée
    bool isAnimated;    // Utilise une spritesheet ?
    int frameCount;     // Nombre de frames d'animation
    float frameSpeed;   // Vitesse (frames par seconde)
    float width;        // Largeur d'une frame (ou 0 pour toute la texture)
    float height;       // Hauteur d'une frame
    float scale;        // Échelle de rendu
    float startX;       // Position X de départ dans la texture
    float startY;       // Position Y de départ dans la texture
};

static std::map<int, EntityRenderConfig> ENTITY_REGISTRY;

Renderer::Renderer(GameState& gameState) : _gameState(gameState)
{
    _textures[0] = LoadTexture("Assets/r-typesheet42.gif");
    _textures[1] = LoadTexture("Assets/attack.png");
    _textures[2] = LoadTexture("Assets/r-typesheet3.gif");
    _textures[3] = LoadTexture("Assets/r-typesheet5.gif");
    _textures[4] = LoadTexture("Assets/r-typesheet1.gif");
    _starTexture = LoadTexture("Assets/star_white_giant01.png");

    for (int i = 0; i < 10; ++i) {
        _stars.push_back({
            static_cast<float>(GetRandomValue(1920, 5000)),
            static_cast<float>(GetRandomValue(0, 1080)),
            5.0f,
            static_cast<float>(GetRandomValue(1, 5)) / 50.0f
        });
    }

    // --- Configuration des Entités (Data-Driven) ---
    if (ENTITY_REGISTRY.empty()) {
        ENTITY_REGISTRY[1] = { 1, false, 1, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };       
        ENTITY_REGISTRY[2] = { 2, true, 12, 8.0f, 17.0f, 18.0f, 2.0f, 0.0f, 0.0f };
        ENTITY_REGISTRY[3] = { 3, true, 8, 12.0f, 33.0f, 36.0f, 2.5f, 0.0f, 0.0f };
        ENTITY_REGISTRY[4] = { 4, true, 4, 0.0f, 29.0f, 30.0f, 3.0f, 136.0f, 19.0f }; 
    }
}

Renderer::~Renderer()
{
    for (auto const& [key, val] : _textures)
    {
        UnloadTexture(val);
    }
    UnloadTexture(_starTexture);
}

void Renderer::draw()
{

    BeginDrawing();

    ClearBackground(BLACK);

    for (auto& star : _stars) {
        star.x -= star.speed;
        if (star.x < -50) {
            star.x = 1920 + 50;
            star.y = GetRandomValue(0, 1080);
        }
        DrawTextureEx(_starTexture, {star.x, star.y}, 0.0f, star.scale, WHITE);
    }

    DrawText("R-Type", 10, 10, 20, DARKGRAY);

    std::string pingText = "Ping: " + std::to_string(_gameState.rtt) + " ms";
    DrawText(pingText.c_str(), 10, 40, 20, DARKGRAY);

    for (const auto& pair : _gameState.players) {
        Color color = (pair.first == _gameState.myPlayerId) ? BLUE : RED;
        
        if (_textures.count(0) && _textures.at(0).id != 0) {
            const Texture2D& texture = _textures.at(0);
            Rectangle sourceRec = { 0.0f, 0.0f, 33.0f, 17.0f };
            Vector2 position = { pair.second.x - sourceRec.width / 2, pair.second.y - sourceRec.height / 2 };
            DrawTextureRec(texture, sourceRec, position, WHITE);
        }

        if (pair.first == _gameState.myPlayerId && IsKeyDown(KEY_SPACE)) {
            if (_textures.count(4) && _textures.at(4).id != 0) {
                const Texture2D& texture = _textures.at(4);
                int currentFrame = static_cast<int>(GetTime() * 10.0f) % 8;
                Rectangle chargeRec = { 0.0f + currentFrame * 33.0f, 49.0f, 33.0f, 36.0f };
                Vector2 chargePos = { pair.second.x + 10.0f, pair.second.y - chargeRec.height / 2 };
                DrawTextureRec(texture, chargeRec, chargePos, WHITE);
            }
        }

        DrawText(std::to_string(pair.first).c_str(), 
                static_cast<int>(pair.second.x - 10),
                static_cast<int>(pair.second.y - 10),
                20, 
                WHITE);
    }

    for (const auto& pair : _gameState.entities) {
        const auto& entity = pair.second;

        if (ENTITY_REGISTRY.count(entity.type)) {
            const auto& config = ENTITY_REGISTRY[entity.type];
            
            if (_textures.count(config.textureId)) {
                const Texture2D& texture = _textures.at(config.textureId);
                Rectangle sourceRec;

                if (config.isAnimated) {
                    int currentFrame = static_cast<int>(GetTime() * config.frameSpeed) % config.frameCount;
                    sourceRec = { config.startX + currentFrame * config.width, config.startY, config.width, config.height };
                } else {
                    float w = (config.width > 0) ? config.width : (float)texture.width;
                    float h = (config.height > 0) ? config.height : (float)texture.height;
                    sourceRec = { config.startX, config.startY, w, h };
                }
                Rectangle destRec = { entity.x, entity.y, sourceRec.width * config.scale, sourceRec.height * config.scale };
                Vector2 origin = { destRec.width / 2, destRec.height / 2 };
                DrawTexturePro(texture, sourceRec, destRec, origin, 0.0f, WHITE);
            }
        } else {
            // Fallback pour entités inconnues (Debug)
            DrawCircleLines(static_cast<int>(entity.x), static_cast<int>(entity.y), 20, MAGENTA);
        }
    }

    EndDrawing();
}

int Renderer::drawRoomMenu(const std::vector<RoomInfo>& rooms)
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("R-Type - Select Room", 50, 50, 40, WHITE);

    Rectangle createBtn = { 50, 120, 250, 50 };
    Vector2 mousePos = GetMousePosition();
    bool hoverCreate = CheckCollisionPointRec(mousePos, createBtn);

    DrawRectangleRec(createBtn, hoverCreate ? LIGHTGRAY : GRAY);
    DrawText("Create Room", 60, 130, 30, BLACK);

    int action = -1;
    if (hoverCreate && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        action = -2;
    }

    int startY = 200;
    for (const auto& room : rooms) {
        Rectangle roomBtn = { 50, (float)startY, 600, 50 };
        bool hoverRoom = CheckCollisionPointRec(mousePos, roomBtn);
        
        DrawRectangleRec(roomBtn, hoverRoom ? DARKGRAY : BLACK);
        DrawRectangleLinesEx(roomBtn, 2, WHITE);

        std::string text = "Room " + std::to_string(room.id) + "   Players: " + 
                           std::to_string(room.playerCount) + "/" + std::to_string(room.maxPlayers);
        DrawText(text.c_str(), 70, startY + 10, 30, WHITE);

        if (hoverRoom && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            action = room.id;
        }
        startY += 60;
    }

    EndDrawing();
    return action;
}

bool Renderer::drawLobby(const LobbyState& lobbyState, uint32_t myPlayerId)
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("Lobby - Waiting for players...", 50, 50, 40, WHITE);

    int startY = 150;
    for (const auto& player : lobbyState.players) {
        std::string text = std::string(player.username) + " (ID: " + std::to_string(player.playerId) + ")";
        if (player.playerId == lobbyState.hostId) {
            text += " - HOST";
        }
        DrawText(text.c_str(), 70, startY, 30, WHITE);
        startY += 40;
    }

    bool startGamePressed = false;
    if (myPlayerId == lobbyState.hostId) {
        Rectangle startBtn = { 50, (float)GetScreenHeight() - 100, 250, 50 };
        Vector2 mousePos = GetMousePosition();
        bool hoverStart = CheckCollisionPointRec(mousePos, startBtn);

        DrawRectangleRec(startBtn, hoverStart ? SKYBLUE : BLUE);
        DrawText("Start Game", 60, GetScreenHeight() - 90, 30, WHITE);

        if (hoverStart && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            startGamePressed = true;
        }
    } else {
        DrawText("Waiting for the host to start the game...", 50, GetScreenHeight() - 100, 20, LIGHTGRAY);
    }
    
    EndDrawing();
    return startGamePressed;
}