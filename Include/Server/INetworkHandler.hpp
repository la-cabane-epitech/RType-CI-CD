/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** INetworkHandler
*/

#ifndef INETWORKHANDLER_HPP_
#define INETWORKHANDLER_HPP_

#include <netinet/in.h>
#include <cstddef>

/**
 * @file INetworkHandler.hpp
 * @brief Interface for handling network messages.
 */

/**
 * @class INetworkHandler
 * @brief Interface defining the contract for handling received network messages.
 *
 * This interface is used to decouple the network reception logic (e.g., UDPServer)
 * from the application logic (e.g., ServerManager or Game).
 */
class INetworkHandler {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~INetworkHandler() = default;

    /**
     * @brief Callback triggered when a message is received.
     *
     * @param data Pointer to the raw data received.
     * @param length Size of the data in bytes.
     * @param addr The source address of the message (UDP).
     */
    virtual void onMessageReceived(const char* data, size_t length, const sockaddr_in& addr) = 0;
};

#endif /* !INETWORKHANDLER_HPP_ */