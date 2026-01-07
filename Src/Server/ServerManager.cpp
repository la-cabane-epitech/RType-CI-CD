/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ServerManager
*/

#include "Server/ServerManager.hpp"
#include <iostream>
#include <thread>
#include <chrono>

ServerManager::ServerManager()
    : _clock(),
      _tcpServer(4242, _rooms, _clock),
      _udpServer(5252, _rooms, _clock),
      _running(true)
{
}

ServerManager::~ServerManager()
{
    _running = false;
    _tcpServer.stop();
    _udpServer.stop();
}

void ServerManager::run()
{
    try {
        _tcpServer.start();
        _udpServer.start();

        std::cout << "[ServerManager] Servers started. Entering game loop..." << std::endl;

        while (_running) {
            for (auto& [id, game] : _rooms) {
                if (game) {
                    game->update(_udpServer);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    } catch (const std::exception& e) {
        std::cerr << "[ServerManager] Error: " << e.what() << std::endl;
    }
}