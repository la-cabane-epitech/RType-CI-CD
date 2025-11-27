/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main.cpp
*/

#include "Client/TCPClient.hpp"
#include "Client/UDPClient.hpp"
#include "Protocole/ProtocoleUDP.hpp"

#include <thread>
#include <chrono>
#include <iostream>
#include "raylib.h"


int main(void)
{
    TCPClient tcpClient("127.0.0.1", 4242);
    if (!tcpClient.connectToServer()) {
        std::cerr << "Impossible de se connecter au serveur TCP\n";
        return 1;
    }

    ConnectResponse res;
    if (!tcpClient.sendConnectRequest("Player1", res)) {
        std::cerr << "Handshake TCP échoué\n";
        return 1;
    }

    std::cout << "Connecté ! PlayerId: " << res.playerId
              << ", UDP Port: " << res.udpPort << "\n";

    UDPClient udpClient("127.0.0.1", res.udpPort);
    uint32_t tick = 0;

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - input keys");

    Vector2 ballPosition = { (float)screenWidth/2, (float)screenHeight/2 };

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        PlayerInputPacket packet{};
        packet.type = UDPMessageType::PLAYER_INPUT;
        packet.playerId = res.playerId;
        packet.tick = tick++;
        packet.inputs = 0;

        if (IsKeyDown(KEY_UP))
            packet.inputs |= UP;
        if (IsKeyDown(KEY_DOWN))
            packet.inputs |= DOWN;
        if (IsKeyDown(KEY_LEFT))
            packet.inputs |= LEFT;
        if (IsKeyDown(KEY_RIGHT))
            packet.inputs |= RIGHT;
        if (IsKeyDown(KEY_SPACE))
            packet.inputs |= SHOOT;

        udpClient.sendMessage(packet);

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("move the ball with arrow keys", 10, 10, 20, DARKGRAY);

            DrawCircleV(ballPosition, 50, MAROON);

        EndDrawing();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    CloseWindow();

    return 0;
}