/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Utils
*/

#ifndef UTILS_HPP_
#define UTILS_HPP_

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    using SocketType = SOCKET;
#else
    #include <unistd.h>
    #include <sys/socket.h>
    using SocketType = int;
#endif

#include <cstddef>
#include <cstdint>
#include <iostream>

namespace NetworkUtils {

/**
 * @brief Receive exactly 'length' bytes from a socket.
 */
inline bool recvAll(SocketType sock, void* buffer, size_t length)
{
    char* buf = static_cast<char*>(buffer);
    size_t total = 0;

    while (total < length) {
#ifdef _WIN32
        int bytes = recv(sock, buf + total, static_cast<int>(length - total), 0);
        if (bytes == SOCKET_ERROR || bytes == 0)
            return false;
#else
        ssize_t bytes = recv(sock, buf + total, length - total, 0);
        if (bytes <= 0)
            return false;
#endif
        total += bytes;
    }
    return true;
}

/**
 * @brief Send exactly 'length' bytes to a socket.
 */
inline bool sendAll(SocketType sock, const void* buffer, size_t length)
{
    const char* buf = static_cast<const char*>(buffer);
    size_t total = 0;

    while (total < length) {
#ifdef _WIN32
        int bytes = send(sock, buf + total, static_cast<int>(length - total), 0);
        if (bytes == SOCKET_ERROR || bytes == 0)
            return false;
#else
        ssize_t bytes = send(sock, buf + total, length - total, 0);
        if (bytes <= 0)
            return false;
#endif
        total += bytes;
    }
    return true;
}

} // namespace NetworkUtils

#endif /* !UTILS_HPP_ */
