/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ServerManager
*/

#ifndef SERVERMANAGER_HPP_
#define SERVERMANAGER_HPP_

#include <map>
#include <memory>
#include <atomic>
#include "Server/Game.hpp"
#include "Server/TCPServer.hpp"
#include "Server/UDPServer.hpp"
#include "Clock.hpp"

/**
 * @class ServerManager
 * @brief Orchestrates the TCP and UDP servers and manages the main game loop.
 */
class ServerManager {
public:
    /**
     * @brief Construct a new Server Manager object.
     * Initializes the TCP and UDP servers.
     */
    ServerManager();
    /**
     * @brief Destroy the Server Manager object.
     * Ensures that the servers are stopped cleanly.
     */
    ~ServerManager();

    /**
     * @brief Starts the servers and enters the main game loop.
     */
    void run();

private:
    Clock _clock; /**< A shared clock for timestamping and synchronization. */
    std::map<int, std::shared_ptr<Game>> _rooms; /**< A map of room IDs to Game instances. */
    TCPServer _tcpServer; /**< The TCP server instance for handling connections and lobbies. */
    UDPServer _udpServer; /**< The UDP server instance for handling real-time game data. */
    std::atomic<bool> _running; /**< Atomic flag to control the main server loop. */
};

#endif /* !SERVERMANAGER_HPP_ */