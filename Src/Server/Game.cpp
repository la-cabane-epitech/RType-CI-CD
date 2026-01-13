/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Game
*/

#include "Server/Game.hpp"
#include <optional>
#include "Server/UDPServer.hpp"
#include <cmath>


void Game::addPlayer(uint32_t playerId, const char* username) {
    std::lock_guard<std::mutex> lock(_playersMutex);
    Player newPlayer{ .id = playerId };
    strncpy(newPlayer.username, username, sizeof(newPlayer.username) - 1);
    _players.push_back(newPlayer);
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

int Game::getPlayerCount()
{
    std::lock_guard<std::mutex> lock(_playersMutex);
    return _players.size();
}

GameStatus Game::getStatus() const
{
    std::lock_guard<std::mutex> lock(_statusMutex);
    return _status;
}

void Game::setStatus(GameStatus status)
{
    std::lock_guard<std::mutex> lock(_statusMutex);
    _status = status;
}

uint32_t Game::getHostId() const
{
    std::lock_guard<std::mutex> lock(_playersMutex);
    return _players.empty() ? 0 : _players.front().id;
}

std::vector<Player> Game::getPlayers() const
{
    std::lock_guard<std::mutex> lock(_playersMutex);
    return _players;
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

void Game::createPlayerChargedShot(uint32_t playerId, UDPServer& udpServer) {
    Player* player = getPlayer(playerId);

    if (!player)
        return;

    std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
    uint32_t entityId = _nextEntityId++;
    _entities.push_back({entityId, 4, player->x + 25, player->y, 12.0f, 0.0f, 29, 30});

    EntitySpawnPacket spawnPkt;
    spawnPkt.entityId = entityId;
    spawnPkt.entityType = 4;
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

    uint16_t type = 2;
    float speed = -5.0f;
    int width = 32;
    int height = 32;

    if (_gameTime > 30.0f) {
        type = 3;
        speed = -8.0f;
        width = 40;
        height = 40;
    }

    _entities.push_back({entityId, type, spawnX, spawnY, speed, 0.0f, width, height});

    EntitySpawnPacket spawnPkt;
    spawnPkt.entityId = entityId;
    spawnPkt.entityType = type;
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
            if (entity.type == 2 || entity.type == 3) {
                entity.velocityY = 5.0f * std::sin(_gameTime * 2.0f + entity.id);
            }
        }
    }
}

void Game::update(UDPServer& udpServer) {
    if (_status != GameStatus::PLAYING)
        return;

    broadcastGameState(udpServer);
    updateEntities(udpServer);
    handleCollision();
    updateGameLevel(0.016f);

    if (std::chrono::steady_clock::now() - _lastEnemySpawnTime > std::chrono::seconds(2)) {
        createEnemy(udpServer);
        _lastEnemySpawnTime = std::chrono::steady_clock::now();
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

void Game::kickPlayer(uint32_t playerId, UDPServer& udpServer)
{
    std::optional<sockaddr_in> kickedPlayerAddr;
    bool playerFoundAndRemoved = false;

    {
        std::lock_guard<std::mutex> lock(_playersMutex);
        auto it = std::find_if(_players.begin(), _players.end(),
                             [playerId](const Player& p) { return p.id == playerId; });

        if (it != _players.end()) {
            if (it->addrSet) {
                kickedPlayerAddr = it->udpAddr;
            }
            _players.erase(it);
            playerFoundAndRemoved = true;
            std::cout << "[Game] Player " << playerId << " was kicked." << std::endl;
        }
    }

    if (playerFoundAndRemoved) {
        // Notify the kicked player
        if (kickedPlayerAddr) {
            YouHaveBeenKickedPacket kickPkt;
            udpServer.queueMessage(kickPkt, *kickedPlayerAddr);
        }

        // Notify remaining players
        PlayerDisconnectPacket disconnectPkt{};
        disconnectPkt.playerId = playerId;
        std::lock_guard<std::mutex> lock(_playersMutex);
        for (const auto& destPlayer : _players) {
            if (destPlayer.addrSet) {
                udpServer.queueMessage(disconnectPkt, destPlayer.udpAddr);
            }
        }
    }
}

void Game::removePlayerFromLobby(uint32_t playerId) {
    std::lock_guard<std::mutex> lock(_playersMutex);
    auto it = std::remove_if(_players.begin(), _players.end(),
                             [playerId](const Player& player) {
                                return player.id == playerId;
                             });
    if (it != _players.end()) {
        _players.erase(it, _players.end());
        std::cout << "[Game] Player " << playerId << " left lobby." << std::endl;
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
        if (projectile.type != 1 && projectile.type != 4) continue;
        for (auto& enemy : _entities) {
            if (enemy.type != 2 && enemy.type != 3) continue;
            if (projectile.is_collide || enemy.is_collide) continue;
            if (checkCollision(projectile.x, projectile.y, projectile.width, projectile.height, enemy.x, enemy.y, enemy.width, enemy.height)) {
                projectile.is_collide = true;
                enemy.is_collide = true;
            }
        }
    }

    for (auto& player : _players) {
        for (auto& enemy : _entities) {
            if (enemy.type != 2 && enemy.type != 3) continue;
            if (enemy.is_collide) continue;

            // Hitbox du joueur (33x17 comme dans le Renderer)
            if (checkCollision(player.x, player.y, 33, 17, enemy.x, enemy.y, enemy.width, enemy.height)) {
                // Le joueur meurt -> Respawn au début
                player.x = 100.0f;
                player.y = 100.0f;
                enemy.is_collide = true;
            }
        }
    }
}
