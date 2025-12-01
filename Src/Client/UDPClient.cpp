/*
** EPITECH PROJECT, 2025
** RType
** File description:
** UDPClient.cpp
*/

#include "Client/UDPClient.hpp"
#include <stdexcept>
#include <iostream>

UDPClient::UDPClient(const std::string& serverIp, uint16_t port)
{
    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sockfd < 0)
        throw std::runtime_error("Failed to create UDP socket");

    // -------- SET NON-BLOCKING SOCKET --------
    fcntl(_sockfd, F_SETFL, O_NONBLOCK);

    // -------- SET SERVER ADDRESS --------
    std::memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIp.c_str(), &_serverAddr.sin_addr) <= 0)
        throw std::runtime_error("Invalid IP address");
}

UDPClient::~UDPClient()
{
    if (_sockfd >= 0)
        close(_sockfd);
}
