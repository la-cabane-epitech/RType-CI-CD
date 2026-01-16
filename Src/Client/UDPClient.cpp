/*
** EPITECH PROJECT, 2025
** RType
** File description:
** UDPClient.cpp
*/

#include "Client/UDPClient.hpp"
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

bool UDPClient::checkConnection()
{
    if (!_socket.is_open()) {
        return true;
    }

    asio::error_code ec;
    asio::socket_base::bytes_readable command(true);
    _socket.io_control(command, ec);

    if (ec) {
        std::cout << "[UDPClient] Connection check failed, assuming disconnected: " << ec.message() << std::endl;
        _socket.close();
        return false;
    }

    return true;
}
