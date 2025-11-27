/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPClient.cpp
*/

#include <iostream>
#include <stdexcept>
#include "Client/UDPClient.hpp"

UDPClient::UDPClient(const std::string& serverIp, uint16_t port) {
    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sockfd < 0) {
        throw std::runtime_error("Failed to create UDP socket");
    }
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIp.c_str(), &_serverAddr.sin_addr);
}

UDPClient::~UDPClient() { if (_sockfd >= 0) close(_sockfd); }

bool UDPClient::sendMessage(const PlayerInputPacket& packet) {
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

bool UDPClient::receivePacket(PlayerStatePacket& packet) {
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