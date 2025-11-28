/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPClient.cpp
*/

#include <iostream>
#include <fcntl.h>
#include <stdexcept>
#include "Client/UDPClient.hpp"
#include <optional>

UDPClient::UDPClient(const std::string& serverIp, uint16_t port) {
    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sockfd < 0) {
        throw std::runtime_error("Failed to create UDP socket");
    }

    // Make socket non-blocking
    fcntl(_sockfd, F_SETFL, O_NONBLOCK);

    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIp.c_str(), &_serverAddr.sin_addr);
}

UDPClient::~UDPClient() { if (_sockfd >= 0) close(_sockfd); }

// Explicit template instantiation
// template bool UDPClient::sendMessage<PlayerInputPacket>(const PlayerInputPacket&);