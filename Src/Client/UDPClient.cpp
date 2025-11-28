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
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("WSAStartup failed");
#endif

    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sockfd < 0)
        throw std::runtime_error("Failed to create UDP socket");

    // -------- SET NON-BLOCKING SOCKET --------
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(_sockfd, FIONBIO, &mode);
#else
    fcntl(_sockfd, F_SETFL, O_NONBLOCK);
#endif

    // -------- SET SERVER ADDRESS --------
    std::memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(port);

#ifdef _WIN32
    if (InetPton(AF_INET, serverIp.c_str(), &_serverAddr.sin_addr) != 1)
        throw std::runtime_error("Invalid IP address");
#else
    if (inet_pton(AF_INET, serverIp.c_str(), &_serverAddr.sin_addr) <= 0)
        throw std::runtime_error("Invalid IP address");
#endif
}

UDPClient::~UDPClient()
{
    if (_sockfd >= 0)
        CLOSESOCK(_sockfd);

#ifdef _WIN32
    WSACleanup();
#endif
}
