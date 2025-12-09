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
#include <iostream>
 #include <algorithm>

#include "CrossPlatformSocket.hpp"
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

struct Entity {
    uint32_t id;
    uint16_t type; // Pour différencier projectiles, ennemis, etc.
    float x;
    float y;
    float velocityX = 10.0f;
    float velocityY = 0.0f;
};

class Game {
public:
    void addPlayer(uint32_t playerId);
    void updatePlayerUdpAddr(uint32_t playerId, const sockaddr_in& udpAddr);
    Player* getPlayer(uint32_t playerId);
    void broadcastGameState(UDPServer& udpServer);

    void createPlayerShot(uint32_t playerId, UDPServer& udpServer);
    void updateEntities(UDPServer& udpServer);

    void disconnectPlayer(uint32_t playerId, UDPServer& udpServer);
private:
    std::vector<Player> _players;
    std::mutex _playersMutex;

    std::vector<Entity> _entities;
    std::mutex _entitiesMutex;
    uint32_t _nextEntityId = 1;
};


#endif /* !GAME_HPP_ */