/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ProtocoleUDP
*/

#ifndef PROTOCOLEUDP_HPP_
    #define PROTOCOLEUDP_HPP_
    #include <cstdint>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <array>

struct ClientInfo {
    sockaddr_in addr;
    uint32_t playerId;
    uint16_t lastSeq;
    uint64_t lastSeen;
};

struct Packet {
    sockaddr_in addr;
    size_t length;
    std::array<char, 512> data;
};

#endif /* !PROTOCOLEUDP_HPP_ */
