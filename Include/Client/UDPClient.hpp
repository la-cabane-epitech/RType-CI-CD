#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

#include <cstring>
#include <string>
#include <optional>
#include <array>
#include <iostream>

#include "Protocole/ProtocoleUDP.hpp"
#include "CrossPlatformSocket.hpp"
#include "Client/Asio.hpp"

/**
 * @class UDPClient
 * @brief Handles UDP communication for real-time game updates.
 */
class UDPClient
{
private:
    asio::io_context _io_context; /**< ASIO IO context */
    asio::ip::udp::socket _socket; /**< UDP socket */
    asio::ip::udp::endpoint _server_endpoint; /**< Endpoint of the server */

public:
    /**
     * @brief Construct a new UDPClient object.
     * @param serverIp IP address of the server.
     * @param port UDP port of the server.
     */
    UDPClient(const std::string& serverIp, uint16_t port);

    /**
     * @brief Destroy the UDPClient object.
     */
    ~UDPClient();

    /**
     * @brief Receives a message from the server.
     * @tparam BufferSize Size of the buffer to use for receiving.
     * @return std::optional<std::array<char, BufferSize>> The received data if successful, nullopt otherwise.
     */
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
            static_cast<int>(buffer.size()),
            0,
            reinterpret_cast<sockaddr*>(&senderAddr),
            &addrLen
        );

        if (received > 0)
            return buffer;

        return std::nullopt;
    }

    /**
     * @brief Sends a packet to the server.
     * @tparam T Type of the packet structure.
     * @param packet The packet to send.
     * @return true if sent successfully, false otherwise.
     */
    template<typename T>
    bool sendMessage(const T& packet) noexcept
    {
        asio::error_code ec;
        _socket.send_to(asio::buffer(&packet, sizeof(T)), _server_endpoint, 0, ec);
        return !ec;
    }

    bool checkConnection();
};

#endif // UDPCLIENT_HPP_
