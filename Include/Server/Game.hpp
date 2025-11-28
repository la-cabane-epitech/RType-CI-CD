/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Game.hpp
*/

#ifndef GAME_HPP_
#define GAME_HPP_

#include <cstdint>
#include <vector>
#include <mutex>
#include <arpa/inet.h>
#include <iostream>

#include "Protocole/ProtocoleUDP.hpp"
class UDPServer;

struct Player {
    uint32_t id;
    float x = 400;
    float y = 225;
    float velocity = 5;
    sockaddr_in udpAddr;
    bool addrSet = false;
};

class Game {
public:
    void addPlayer(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_playersMutex);
        _players.push_back({playerId});
    }

    void updatePlayerUdpAddr(uint32_t playerId, const sockaddr_in& udpAddr) {
        std::lock_guard<std::mutex> lock(_playersMutex);
        for (auto& player : _players) {
            if (player.id == playerId) {
                if (!player.addrSet) {
                    player.udpAddr = udpAddr;
                    player.addrSet = true;
                    std::cout << "[Game] Player " << playerId << " UDP address set." << std::endl;
                }
                return;
            }
        }
    }

    Player* getPlayer(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_playersMutex);
        for (auto& player : _players) {
            if (player.id == playerId) {
                return &player;
            }
        }
        return nullptr;
    }

    void broadcastGameState(UDPServer& udpServer);

private:
    std::vector<Player> _players;
    std::mutex _playersMutex;
};


#endif /* !GAME_HPP_ */