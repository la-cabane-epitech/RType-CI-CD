/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main.cpp
*/

#include "../../Include/Client/TCPClient.hpp"
#include "../../Include/Client/UDPClient.hpp"
#include <iostream>

int main() {

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

    while (true) {
        Packet packet;
        packet.type = 1;
        udpClient.sendMessage(packet);

        Packet recvPacket;

        if (udpClient.receivePacket(recvPacket)) {
            std::cout << "Packet reçu ! type=" << int(recvPacket.type) << "\n";
        }

    }

    return 0;
}