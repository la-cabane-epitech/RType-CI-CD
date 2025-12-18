/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Game
*/

#include "Server/Game.hpp"
#include "Server/UDPServer.hpp"
#include <cmath>


void Game::addPlayer(uint32_t playerId) {
    std::lock_guard<std::mutex> lock(_playersMutex);
    _players.push_back({playerId});
}

void Game::broadcastGameState(UDPServer& udpServer) {
    std::lock_guard<std::mutex> lock(_playersMutex);

    for (const auto& player : _players) {
        if (!player.addrSet) continue;

        PlayerStatePacket statePkt;
        statePkt.playerId = player.id;
        statePkt.lastProcessedTick = player.lastProcessedTick;
        statePkt.x = player.x;
        statePkt.y = player.y;

        for (const auto& destPlayer : _players) {
            if (!destPlayer.addrSet) continue;
            udpServer.queueMessage(statePkt, destPlayer.udpAddr);
        }
    }
}

void Game::updatePlayerUdpAddr(uint32_t playerId, const sockaddr_in& udpAddr) {
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

void Game::setPlayerLastProcessedTick(uint32_t playerId, uint32_t tick) {
    std::lock_guard<std::mutex> lock(_playersMutex);
    for (auto& player : _players) {
        if (player.id == playerId) {
            if (tick > player.lastProcessedTick)
                player.lastProcessedTick = tick;
            return;
        }
    }
}

Player* Game::getPlayer(uint32_t playerId) {
    std::lock_guard<std::mutex> lock(_playersMutex);
    for (auto& player : _players) {
        if (player.id == playerId) {
            return &player;
        }
    }
    return nullptr;
}

void Game::createPlayerShot(uint32_t playerId, UDPServer& udpServer) {
    Player* player = getPlayer(playerId);

    if (!player)
        return;

    std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
    uint32_t entityId = _nextEntityId++;
    _entities.push_back({entityId, 1, player->x + 25, player->y, 10.0f, 0.0f, 10, 5});

    EntitySpawnPacket spawnPkt;
    spawnPkt.entityId = entityId;
    spawnPkt.entityType = 1;
    spawnPkt.x = player->x + 25;
    spawnPkt.y = player->y;

    std::lock_guard<std::mutex> lock_players(_playersMutex);
    for (const auto& destPlayer : _players) {
        if (destPlayer.addrSet) {
            udpServer.queueMessage(spawnPkt, destPlayer.udpAddr);
        }
    }
}

void Game::createEnemy(UDPServer& udpServer) {
    std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
    uint32_t entityId = _nextEntityId++;

    float spawnX = 1920.0f;
    float spawnY = rand() % 1000 + 40;

    _entities.push_back({entityId, 2, spawnX, spawnY, -5.0f, 0.0f, 32, spawnX, 32});

    EntitySpawnPacket spawnPkt;
    spawnPkt.entityId = entityId;
    spawnPkt.entityType = 2;
    spawnPkt.x = spawnX;
    spawnPkt.y = spawnY;

    std::lock_guard<std::mutex> lock_players(_playersMutex);
    for (const auto& destPlayer : _players) {
        if (destPlayer.addrSet) {
            udpServer.queueMessage(spawnPkt, destPlayer.udpAddr);
        }
    }
} 

void Game::updateEntities(UDPServer& udpServer) {
    std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
    std::vector<uint32_t> destroyedEntities;

    for (auto it = _entities.begin(); it != _entities.end(); ) {
        auto& entity = *it;
        entity.x += entity.velocityX;
        entity.y += entity.velocityY;

        if (entity.x > 1920 || entity.x < -20 || entity.is_collide) {
            destroyedEntities.push_back(entity.id);
            EntityDestroyPacket destroyPkt;
            destroyPkt.entityId = entity.id;
            for (const auto& destPlayer : _players) {
                if (destPlayer.addrSet)
                    udpServer.queueMessage(destroyPkt, destPlayer.udpAddr);
            }
            it = _entities.erase(it);
        } else {
            EntityUpdatePacket updatePkt;
            updatePkt.entityId = entity.id;
            updatePkt.x = entity.x;
            updatePkt.y = entity.y;

            std::lock_guard<std::mutex> lock_players(_playersMutex);
            for (const auto& destPlayer : _players) {
                if (destPlayer.addrSet) {
                    udpServer.queueMessage(updatePkt, destPlayer.udpAddr);
                }
            }
            ++it;
        }
    }
}

void Game::updateGameLevel(float elapsedTime) {
    _gameTime += elapsedTime;

    if (_gameTime > 60.0f) {
        std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
        for (auto& entity : _entities) {
            if (entity.type == 2) {
                entity.velocityY = 5.0f * std::sin(_gameTime * 2.0f + entity.id);
            }
        }
    }
}

void Game::disconnectPlayer(uint32_t playerId, UDPServer& udpServer) {
    std::lock_guard<std::mutex> lock(_playersMutex);
    auto it = std::remove_if(_players.begin(), _players.end(),
                             [playerId](const Player& player) {
                                return player.id == playerId;
                             });
    if (it != _players.end()) {
        _players.erase(it, _players.end());
        std::cout << "[Game] Player " << playerId << " disconnected." << std::endl;
    }

    for (const auto& destPlayer : _players) {
        if (destPlayer.addrSet) {
            PlayerDisconnectPacket disconnectPkt {};
            disconnectPkt.playerId = playerId;
            udpServer.queueMessage(disconnectPkt, destPlayer.udpAddr);
            std::cout << "Send message disconnect to player " << destPlayer.id << "." << std::endl;
        }
    }
}

bool Game::checkCollision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2) {
    return  x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2;
}

void Game::handleCollision() {
    std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
    std::lock_guard<std::mutex> lock_players(_playersMutex);

    for (auto& projectile : _entities) {
        if (projectile.type != 1) continue;
        for (auto& enemy : _entities) {
            if (enemy.type != 2) continue;
            if (projectile.is_collide || enemy.is_collide) continue;
            if (checkCollision(projectile.x, projectile.y, projectile.width, projectile.height, enemy.x, enemy.y, enemy.width, enemy.height)) {
                projectile.is_collide = true;
                enemy.is_collide = true;
            }
        }
    }
}
