/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** CrossPlatformSocket.hpp
*/

#ifndef CROSSPLATFORMSOCKET_HPP_
#define CROSSPLATFORMSOCKET_HPP_

/**
 * @file CrossPlatformSocket.hpp
 * @brief Abstraction layer for socket headers on Windows and Linux.
 */

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#endif // CROSSPLATFORMSOCKET_HPP_