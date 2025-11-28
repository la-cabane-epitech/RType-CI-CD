#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

// -------- PLATFORM ABSTRACTION --------
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    using socklen_t = int;
    using ssize_t = long long;
    #define CLOSESOCK closesocket
    #define GET_ERROR WSAGetLastError()
#else
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
    #define CLOSESOCK close
    #define GET_ERROR errno
    using SOCKET = int;
#endif

#include <cstring>
#include <string>
#include <optional>
#include <array>

#include "Protocole/ProtocoleUDP.hpp"
#include "CrossPlatformSocket.hpp"

class UDPClient
{
private:
    SOCKET _sockfd;
    sockaddr_in _serverAddr;

public:
    UDPClient(const std::string& serverIp, uint16_t port);
    ~UDPClient();

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

    template<typename T>
    bool sendMessage(const T& packet)
    {
        ssize_t sent = sendto(
            _sockfd,
            reinterpret_cast<const char *>(&packet),
            sizeof(T),
            0,
            (sockaddr*)&_serverAddr,
            sizeof(_serverAddr)
        );

        return sent == sizeof(T);
    }
};

#endif // UDPCLIENT_HPP_
