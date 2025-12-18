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
    uint32_t lastProcessedTick = 0;
    bool addrSet = false;
    int height = 32;
    int width = 32;
};

struct Entity {
    uint32_t id;
    uint16_t type;
    float x;
    float y;
    float velocityX = 10.0f;
    float velocityY = 0.0f;
    int height = 0;
    float initialX;
    int width = 0;
    bool is_collide = false;
};

class Game {
public:
    void addPlayer(uint32_t playerId);
    void updatePlayerUdpAddr(uint32_t playerId, const sockaddr_in& udpAddr);
    Player* getPlayer(uint32_t playerId);
    void broadcastGameState(UDPServer& udpServer);
    void setPlayerLastProcessedTick(uint32_t playerId, uint32_t tick);

    void createPlayerShot(uint32_t playerId, UDPServer& udpServer);
    void updateEntities(UDPServer& udpServer);
    void createEnemy(UDPServer& udpServer);
    void disconnectPlayer(uint32_t playerId, UDPServer& udpServer);
    void handleCollision();
    void updateGameLevel(float elapsedTime);
private:
    std::vector<Player> _players;
    std::mutex _playersMutex;

    std::vector<Entity> _entities;
    std::mutex _entitiesMutex;
    uint32_t _nextEntityId = 1;
    bool checkCollision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);
    float _gameTime = 0.0f;
};


#endif /* !GAME_HPP_ */
