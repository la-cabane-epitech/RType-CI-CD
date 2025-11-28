/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Network
*/

#ifndef RTYPE_NETWORK_HPP
#define RTYPE_NETWORK_HPP

#include <stdexcept>
#include <string>

// Platform-specific includes and definitions
#ifdef _WIN32
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <winsock2.h>
    #include <ws2tcpip.h>
    // Link with Ws2_32.lib
    #pragma comment(lib, "Ws2_32.lib")

    using SocketType = SOCKET;
    const SocketType INVALID_SOCKET_VAL = INVALID_SOCKET;
    #define GET_LAST_ERROR() WSAGetLastError()
    #define CLOSE_SOCKET(s) closesocket(s)
#else // For Linux and other POSIX systems
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <cerrno>

    using SocketType = int;
    const SocketType INVALID_SOCKET_VAL = -1;
    #define GET_LAST_ERROR() errno
    #define CLOSE_SOCKET(s) close(s)
#endif

// RAII class for network library initialization and cleanup.
// On Windows, it calls WSAStartup/WSACleanup. On Linux, it does nothing.
class NetworkInitializer {
public:
    NetworkInitializer() {
#ifdef _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            throw std::runtime_error("WSAStartup failed with error: " + std::to_string(result));
        }
#endif
    }

    ~NetworkInitializer() {
#ifdef _WIN32
        WSACleanup();
#endif
    }
};


#endif //RTYPE_NETWORK_HPP