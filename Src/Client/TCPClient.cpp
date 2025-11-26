/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPClient.cpp
*/

#include "../../Include/Client/TCPClient.hpp"

TCPClient::TCPClient(const std::string& serverIp, uint16_t port)
    : _serverIp(serverIp), _port(port), _sock(-1) {}

TCPClient::~TCPClient() {
    if (_sock != -1) {
        close(_sock);
        _sock = -1;
    }
}

bool TCPClient::connectToServer() {
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0) {
        perror("socket");
        return false;
    }

    sockaddr_in servAddr {};
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _serverIp.c_str(), &servAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address\n";
        return false;
    }

    if (connect(_sock, (sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("connect");
        return false;
    }

    return true;
}

bool TCPClient::sendConnectRequest(const std::string& username, ConnectResponse& outResponse) {
    ConnectRequest req {};
    req.type = 1;
    strncpy(req.username, username.c_str(), sizeof(req.username)-1);
    req.username[sizeof(req.username)-1] = '\0';

    if (send(_sock, &req, sizeof(req), 0) <= 0) {
        perror("send");
        return false;
    }

    uint8_t type;
    if (recv(_sock, &type, 1, 0) <= 0) {
        perror("recv");
        return false;
    }

    if (type == 2) {
        outResponse.type = type;
        if (recv(_sock, ((char*)&outResponse) + 1, sizeof(outResponse)-1, 0) <= 0) {
            perror("recv");
            return false;
        }
        return true;
    } else if (type == 3) {
        ErrorResponse err {};
        err.type = type;
        if (recv(_sock, ((char*)&err) + 1, sizeof(err)-1, 0) <= 0) {
            perror("recv");
        }
        std::cerr << "Server Error: " << err.message << "\n";
        return false;
    }

    return false;
}