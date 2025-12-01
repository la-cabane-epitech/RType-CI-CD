#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

#include <cstring>
#include <string>
#include <optional>
#include <array>

#include "Protocole/ProtocoleUDP.hpp"
#include "CrossPlatformSocket.hpp"
#include "asio.hpp"

class UDPClient
{
private:
    asio::io_context _io_context;
    asio::ip::udp::socket _socket;
    asio::ip::udp::endpoint _server_endpoint;

public:
    UDPClient(const std::string& serverIp, uint16_t port);
    ~UDPClient();

    template<size_t BufferSize>
    std::optional<std::array<char, BufferSize>> receiveMessage() noexcept
    {
        if (_socket.native_handle() < 0)
            return std::nullopt;

        std::array<char, BufferSize> buffer{};
        sockaddr_in senderAddr{};
        socklen_t addrLen = sizeof(senderAddr);

        ssize_t received = recvfrom(
            _socket.native_handle(),
            buffer.data(),
            buffer.size(),
            0,
            (sockaddr*)&senderAddr,
            &addrLen
        );

        if (received > 0)
            return buffer;

        return std::nullopt;
    }

    template<typename T>
    bool sendMessage(const T& packet) noexcept
    {
        asio::error_code ec;
        _socket.send_to(asio::buffer(&packet, sizeof(T)), _server_endpoint, 0, ec);
        return !ec;
    }
};

#endif // UDPCLIENT_HPP_
