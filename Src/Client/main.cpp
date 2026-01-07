/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/

#include "Client/Ray.hpp"
#include "Client/TCPClient.hpp"
#include "Client/RTypeClient.hpp"
#include <iostream>

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " <server_ip>\n";
        return 1;
    }
    std::string serverIp = av[1];

    TCPClient tcpClient(serverIp, 4242);
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

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "R-Type Client");
    SetTargetFPS(60);

    bool inRoom = false;
    GameState menuState;
    Renderer menuRenderer(menuState);

    {
        std::vector<RoomInfo> rooms;
        double lastUpdate = 0;

        while (!WindowShouldClose() && !inRoom) {
            double now = GetTime();
            if (now - lastUpdate > 1.0) {
                rooms = tcpClient.getRooms();
                lastUpdate = now;
            }

            int action = menuRenderer.drawRoomMenu(rooms);
            if (action == -2) {
                int newRoom = tcpClient.createRoom();
                if (newRoom >= 0 && tcpClient.joinRoom(newRoom)) inRoom = true;
            } else if (action >= 0) {
                if (tcpClient.joinRoom(action)) inRoom = true;
            }
        }
    }

    bool gameStarted = false;
    if (inRoom && !WindowShouldClose()) {
        LobbyState lobbyState;
        double lastLobbyUpdate = 0;

        while(!WindowShouldClose() && !gameStarted) {
            if (GetTime() - lastLobbyUpdate > 0.5) {
                lobbyState = tcpClient.getLobbyState();
                lastLobbyUpdate = GetTime();
            }

            if (lobbyState.gameIsStarting) {
                gameStarted = true;
                continue;
            }

            if (menuRenderer.drawLobby(lobbyState, res.playerId)) {
                tcpClient.sendStartGameRequest();
            }
        }
    }

    if (gameStarted && !WindowShouldClose()) {
        RTypeClient client(serverIp, res);
        client.run();
    }

    CloseWindow();
    return 0;
}
