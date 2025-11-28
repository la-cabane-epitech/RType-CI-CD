/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/

#include "Client/TCPClient.hpp"
#include "Client/RTypeClient.hpp"
#include <iostream>
#include "raylib.h"


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

    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "R-Type Client");
    SetTargetFPS(60);

    RTypeClient client(serverIp, res);
    client.run();

    CloseWindow();

    return 0;
}