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
    ServerManager();
    ~ServerManager();

    /**
     * @brief Starts the servers and enters the main game loop.
     */
    void run();

private:
    Clock _clock;
    std::map<int, std::shared_ptr<Game>> _rooms; /**< Shared map of game rooms */
    TCPServer _tcpServer;
    UDPServer _udpServer;
    std::atomic<bool> _running;
};

#endif /* !SERVERMANAGER_HPP_ */