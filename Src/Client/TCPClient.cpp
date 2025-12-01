/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPClient.cpp
*/

#include "Client/TCPClient.hpp"
#include <iostream>
#include <stdexcept>

TCPClient::TCPClient(const std::string& serverIp, uint16_t port)
    : _serverIp(serverIp), _port(port), _sock(-1)
{
}

TCPClient::~TCPClient()
{
    if (_sock != -1) {
        close(_sock);
        _sock = -1;
    }
}

bool TCPClient::connectToServer()
{
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0) {
        std::cerr << "Failed to create socket\n";
        return false;
    }

    sockaddr_in servAddr{};
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _serverIp.c_str(), &servAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address\n";
        return false;
    }

    if (connect(_sock, (sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        std::cerr << "Connection failed\n";
        return false;
    }

    return true;
}

bool TCPClient::sendConnectRequest(const std::string& username, ConnectResponse& outResponse)
{
    ConnectRequest req{};
    req.type = 1;
    std::strncpy(req.username, username.c_str(), sizeof(req.username) - 1);

    // -------- SEND FULL REQUEST --------
    int sent = send(_sock, reinterpret_cast<const char*>(&req), sizeof(req), 0);
    if (sent <= 0) {
        std::cerr << "Failed to send\n";
        return false;
    }

    // -------- RECEIVE PACKET TYPE --------
    uint8_t type = 0;
    if (recv(_sock, reinterpret_cast<char*>(&type), 1, 0) <= 0) {
        std::cerr << "Failed to receive packet type\n";
        return false;
    }

    // -------- CONNECT RESPONSE --------
    if (type == 2) {
        outResponse.type = 2;
        if (recv(_sock, reinterpret_cast<char*>(&outResponse) + 1,
                sizeof(outResponse) - 1, 0) <= 0) {
            std::cerr << "Failed to receive response\n";
            return false;
        }
        return true;
    }

    // -------- ERROR RESPONSE --------
    if (type == 3) {
        ErrorResponse err{};
        err.type = 3;

        recv(_sock, reinterpret_cast<char*>(&err) + 1,
            sizeof(err) - 1, 0);

        std::cerr << "Server Error: " << err.message << "\n";
        return false;
    }

    return false;
}
