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

struct Packet {
    uint32_t playerId;
    uint8_t type;
    float x;
    float y;
};

class UDPClient
{
private:
    int _sockfd;
    sockaddr_in _serverAddr;
public:
    UDPClient(const std::string& serverIp, uint16_t port) {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    }

    bool sendMessage(const Packet& packet);
    bool receivePacket(Packet& packet);
};

#endif // UDPCLIENT_HPP_