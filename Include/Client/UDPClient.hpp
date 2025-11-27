/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPClient.hpp
*/

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>

#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

#include "Protocole/ProtocoleUDP.hpp"

class UDPClient
{
private:
    int _sockfd;
    sockaddr_in _serverAddr;
public:
    UDPClient(const std::string& serverIp, uint16_t port);
    ~UDPClient();

    bool sendMessage(const PlayerInputPacket& packet);
    bool receivePacket(PlayerStatePacket& packet);
};

#endif // UDPCLIENT_HPP_