#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <optional>
#include <array>

#include "Protocole/ProtocoleUDP.hpp"

class UDPClient
{
private:
    int _sockfd;
    sockaddr_in _serverAddr;

public:
    UDPClient(const std::string& serverIp, uint16_t port);
    ~UDPClient();

    // bool sendMessage(const PlayerInputPacket& packet);
    // bool receivePacket(PlayerStatePacket& packet);

    // ----------- TEMPLATE RECEIVE RAW BUFFER -----------
    template<size_t BufferSize>
    std::optional<std::array<char, BufferSize>> receiveMessage()
    {
        if (_sockfd < 0)
            return std::nullopt;

        std::array<char, BufferSize> buffer{};
        sockaddr_in senderAddr{};
        socklen_t addrLen = sizeof(senderAddr);

        ssize_t received = recvfrom(
            _sockfd,
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

    // ----------- TEMPLATE SEND ANY PACKET -----------
    template<typename T>
    bool sendMessage(const T& packet)
    {
        if (_sockfd < 0)
            return false;

        ssize_t sent = sendto(
            _sockfd,
            &packet,
            sizeof(T),
            0,
            (sockaddr*)&_serverAddr,
            sizeof(_serverAddr)
        );

        return sent == sizeof(T);
    }
};

#endif // UDPCLIENT_HPP_
