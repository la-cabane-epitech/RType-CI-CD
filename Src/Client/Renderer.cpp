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
    _textures[0] = LoadTexture("Assets/r-typesheet42.gif");
    _textures[1] = LoadTexture("Assets/attack.png");
    _textures[2] = LoadTexture("Assets/r-typesheet3.gif");
    _starTexture = LoadTexture("Assets/star_white_giant01.png");

    for (int i = 0; i < 10; ++i) {
        _stars.push_back({
            static_cast<float>(GetRandomValue(1920, 5000)),
            static_cast<float>(GetRandomValue(0, 1080)),
            5.0f,
            static_cast<float>(GetRandomValue(1, 5)) / 50.0f
        });
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
            Vector2 position = { pair.second.x - 33.0f / 2, pair.second.y - 17.0f / 2 };
            DrawTextureRec(texture, sourceRec, position, WHITE);
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
            if (_textures.count(2)) {
                const Texture2D& texture = _textures.at(2);
                float frameWidth = 17.0f;
                float frameHeight = 18.0f;
                int currentFrame = static_cast<int>(GetTime() * 8.0f) % 12;
                Rectangle sourceRec = { currentFrame * frameWidth, 0.0f, frameWidth, frameHeight };
                Vector2 position = { entity.x - frameWidth / 2, entity.y - frameHeight / 2 };
                DrawTextureRec(texture, sourceRec, position, WHITE);
            }
        }
        else if (_textures.count(entity.type)) {
            DrawTexture(_textures.at(entity.type), static_cast<int>(entity.x), static_cast<int>(entity.y), WHITE);
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