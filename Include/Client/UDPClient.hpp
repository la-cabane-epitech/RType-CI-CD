/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPClient.hpp
*/

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>     // pour sendto et recvfrom
#include <arpa/inet.h>  // pour sockaddr_in
#include <cstring>
#include <string>

#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

#pragma pack(push, 1)
struct Packet {
    uint32_t playerId;
    uint8_t type;
    float x;
    float y;
};
#pragma pack(pop)

class UDPClient
{
private:
    int _sockfd;
    sockaddr_in _serverAddr;
public:
    UDPClient(const std::string& serverIp, uint16_t port) {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0) {
            throw std::runtime_error("Failed to create UDP socket");
        }
        _serverAddr.sin_family = AF_INET;
        _serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, serverIp.c_str(), &_serverAddr.sin_addr);
    }

    bool sendMessage(const Packet& packet);
    bool receivePacket(Packet& packet);
};

#endif // UDPCLIENT_HPP_