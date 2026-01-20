/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Packet
*/

#ifndef NETWORK_PACKET_HPP_
#define NETWORK_PACKET_HPP_

#include <array>
#include <cstddef>
#include "CrossPlatformSocket.hpp"

/**
 * @file Packet.hpp
 * @brief Defines the generic Packet structure for network communication.
 */

namespace Network {

/**
 * @struct Packet
 * @brief Represents a raw data packet received or to be sent via UDP.
 */
struct Packet {
    static constexpr size_t MAX_SIZE = 1024; ///< Maximum payload size
    std::array<char, MAX_SIZE> data;         ///< Raw data buffer
    size_t length = 0;                       ///< Actual length of data in buffer
    sockaddr_in addr;                        ///< Source or destination address
};

}

#endif /* !NETWORK_PACKET_HPP_ */