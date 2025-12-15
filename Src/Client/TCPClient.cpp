/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPClient.cpp
*/

#include "Client/TCPClient.hpp"
#include <iostream>

TCPClient::TCPClient(const std::string& serverIp, uint16_t port)
    : _io_context(), _socket(_io_context), _serverIp(serverIp), _port(port)
{
}

TCPClient::~TCPClient()
{
    if (_socket.is_open()) {
        _socket.close();
    }
}

bool TCPClient::connectToServer()
{
    try {
        asio::ip::tcp::resolver resolver(_io_context);
        auto endpoints = resolver.resolve(_serverIp, std::to_string(_port));
        asio::connect(_socket, endpoints);
    } catch (const std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool TCPClient::sendConnectRequest(const std::string& username, ConnectResponse& outResponse)
{
    try {
        ConnectRequest req{};
        req.type = 1;
        std::strncpy(req.username, username.c_str(), sizeof(req.username) - 1);
        req.username[sizeof(req.username) - 1] = '\0';

        asio::write(_socket, asio::buffer(&req, sizeof(req)));

        uint8_t type = 0;
        asio::read(_socket, asio::buffer(&type, 1));

        if (type == 2) {
            outResponse.type = 2;
            asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&outResponse) + 1, sizeof(outResponse) - 1));
            return true;
        }

        if (type == 3) {
            ErrorResponse err{};
            err.type = 3;

            asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&err) + 1, sizeof(err) - 1));

            std::cerr << "Server Error: " << err.message << "\n";
            return false;
        }

        std::cerr << "Invalid response type from server\n";
        return false;

    } catch (const std::exception& e) {
        std::cerr << "Handshake failed: " << e.what() << "\n";
        return false;
    }
}
