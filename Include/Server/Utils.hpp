/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Utils
*/

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <unistd.h>
#include <cstddef>
#include <cstdint>
#include <sys/socket.h>
#include <iostream>

namespace NetworkUtils {

/**
 * @brief Receive exactly 'length' bytes from a socket.
 * @param sock Socket file descriptor
 * @param buffer Destination buffer
 * @param length Number of bytes to read
 * @return true if all bytes were received, false on error or disconnection
 */
inline bool recvAll(int sock, void* buffer, size_t length) {
    char* buf = static_cast<char*>(buffer);
    size_t total = 0;

    while (total < length) {
        ssize_t bytes = recv(sock, buf + total, length - total, 0);
        if (bytes <= 0) {
            // error or client disconnected
            return false;
        }
        total += bytes;
    }
    return true;
}

/**
 * @brief Send exactly 'length' bytes to a socket.
 * @param sock Socket file descriptor
 * @param buffer Source buffer
 * @param length Number of bytes to send
 * @return true if all bytes were sent, false on error
 */
inline bool sendAll(int sock, const void* buffer, size_t length) {
    const char* buf = static_cast<const char*>(buffer);
    size_t total = 0;

    while (total < length) {
        ssize_t bytes = send(sock, buf + total, length - total, 0);
        if (bytes <= 0) {
            return false;
        }
        total += bytes;
    }
    return true;
}

} // namespace NetworkUtils


#endif /* !UTILS_HPP_ */
