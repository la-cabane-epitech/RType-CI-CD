/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Game
*/

#include "Server/Game.hpp"
#include "Server/UDPServer.hpp"


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
    _entities.push_back({entityId, 1, player->x + 25, player->y, 10.0f, 0.0f});

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
 
    _entities.push_back({entityId, 2, spawnX, spawnY, -5.0f, 0.0f});
 
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

        Supprime l'entité si elle sort de l'écran
        // if (entity.x > 850) {
        if (entity.x > 1920 || entity.x < -20) {
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
