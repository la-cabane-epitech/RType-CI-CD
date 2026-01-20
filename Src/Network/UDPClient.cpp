/*
** EPITECH PROJECT, 2025
** RType
** File description:
** UDPClient.cpp
*/

#include "Network/UDP/UDPClient.hpp"
#include <stdexcept>

UDPClient::UDPClient(const std::string& serverIp, uint16_t port)
    : _io_context(), _socket(_io_context)
{
    asio::ip::udp::resolver resolver(_io_context);
    _server_endpoint = *resolver.resolve(asio::ip::udp::v4(), serverIp, std::to_string(port)).begin();

    _socket.open(asio::ip::udp::v4());
    _socket.non_blocking(true);
}

UDPClient::~UDPClient()
{
    if (_socket.is_open()) {
        _socket.close();
    }
}
