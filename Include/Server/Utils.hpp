/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Utils
*/

#ifndef UTILS_HPP_
#define UTILS_HPP_

    #include <unistd.h>
    #include <sys/socket.h>

#include <cstddef>
#include <cstdint>
#include <iostream>

/**
 * @file Utils.hpp
 * @brief Utility functions for socket operations.
 */

/**
 * @namespace NetworkUtils
 * @brief Contains helper functions for network communication.
 */
namespace NetworkUtils {

/**
 * @brief Receive exactly 'length' bytes from a socket.
 * @param sock The socket file descriptor.
 * @param buffer Pointer to the buffer to store received data.
 * @param length Number of bytes to receive.
 * @return true if all bytes were received, false on error or connection close.
 */
inline bool recvAll(int sock, void* buffer, size_t length)
{
    char* buf = static_cast<char*>(buffer);
    size_t total = 0;

    while (total < length) {
        ssize_t bytes = recv(sock, buf + total, length - total, 0);
        if (bytes <= 0)
            return false;
        total += bytes;
    }
    return true;
}

/**
 * @brief Send exactly 'length' bytes to a socket.
 * @param sock The socket file descriptor.
 * @param buffer Pointer to the data to send.
 * @param length Number of bytes to send.
 * @return true if all bytes were sent, false on error.
 */
inline bool sendAll(int sock, const void* buffer, size_t length)
{
    const char* buf = static_cast<const char*>(buffer);
    size_t total = 0;

    while (total < length) {
        ssize_t bytes = send(sock, buf + total, length - total, 0);
        if (bytes <= 0)
            return false;
        total += bytes;
    }
    return true;
}

}

#endif /* !UTILS_HPP_ */
