/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Packet
*/

#ifndef PACKET_HPP_
#define PACKET_HPP_

#include <array>
#include <cstddef>
#include "CrossPlatformSocket.hpp"

/**
 * @struct Packet
 * @brief Represents a raw data packet received or to be sent via UDP.
 */
struct Packet {
    static constexpr size_t MAX_SIZE = 1024;
    std::array<char, MAX_SIZE> data;
    size_t length = 0;
    sockaddr_in addr;
};


#endif /* !PACKET_HPP_ */