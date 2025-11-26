/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPClient.cpp
*/

#include <iostream>
#include "Client/UDPClient.hpp"

bool UDPClient::sendMessage(const Packet& packet) {
    if (_sockfd < 0)
            return false;

        size_t sent = sendto(
            _sockfd,
            &packet,
            sizeof(packet),
            0,
            (sockaddr*)&_serverAddr,
            sizeof(_serverAddr)
        );
        std::cout << "UDPClient: Sent " << sent << " bytes\n";
        return sent == sizeof(packet);
}

bool UDPClient::receivePacket(Packet& packet) {
    if (_sockfd < 0)
        return false;

    sockaddr_in senderAddr;
    socklen_t addrLen = sizeof(senderAddr);

    ssize_t received = recvfrom(
        _sockfd,
        &packet,                  // où stocker le paquet reçu
        sizeof(packet),           // taille maximale
        0,
        (struct sockaddr*)&senderAddr,
        &addrLen
    );

    return received == sizeof(packet);
}