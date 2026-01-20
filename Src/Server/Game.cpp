/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Game
*/
#include <chrono>

#include "Server/Game.hpp"
#include "Network/UDP/UDPServer.hpp"
#include <cmath>
#include <unordered_map>

// Static maps to hold Boss state per Game instance since we cannot modify Game.hpp
static std::unordered_map<Game*, int> g_bossHP;
static std::unordered_map<Game*, int> g_bossLevel; // 0: None, 1: Boss1, 2: Cooldown, 3: Boss2, 4: Victory
static std::unordered_map<Game*, float> g_lastBossShootTime;
static std::unordered_map<Game*, float> g_bossDeathTime;

void Game::addPlayer(uint32_t playerId, const char* username) {
    std::lock_guard<std::mutex> lock(_playersMutex);
    Player newPlayer{ .id = playerId };
    strncpy(newPlayer.username, username, sizeof(newPlayer.username) - 1);
    _players.push_back(newPlayer);
}

void Game::handlePlayerInput(const PlayerInputPacket& pkt, UDPServer& udpServer)
{
    Player* player = getPlayer(pkt.playerId);
    if (!player) {
        return;
    }

    // --- Détection de la perte de paquets ---
    if (player->firstInputReceived) {
        player->lastInputTick = pkt.tick;
        player->receivedInputs = 1;
        player->firstInputReceived = false;
    } else if (pkt.tick > player->lastInputTick) {
        uint32_t lostCount = pkt.tick - player->lastInputTick - 1;
        if (lostCount > 0) {
            player->lostInputs += lostCount;
            // Optionnel: logguer une perte excessive
            // std::cout << "[GAME] Player " << pkt.playerId << " lost " << lostCount << " input packets." << std::endl;
        }
        player->receivedInputs++;
        player->lastInputTick = pkt.tick;
    }
    // else: paquet arrivé en désordre ou dupliqué, on l'ignore pour les stats pour l'instant.

    // --- Logique de traitement de l'input (déplacée ici) ---
    setPlayerLastProcessedTick(pkt.playerId, pkt.tick);

    if (pkt.inputs & UP) player->y -= player->velocity;
    if (pkt.inputs & DOWN) player->y += player->velocity;
    if (pkt.inputs & LEFT) player->x -= player->velocity;
    if (pkt.inputs & RIGHT) player->x += player->velocity;
    if (pkt.inputs & PRESSED) createPlayerShot(pkt.playerId, udpServer);
    if (pkt.inputs & HOLD) createPlayerChargedShot(pkt.playerId, udpServer);
}

void Game::broadcastGameState(UDPServer& udpServer) {
    std::lock_guard<std::mutex> lock(_playersMutex);

    for (auto& player : _players) { // Doit être non-const pour modifier la séquence
        if (!player.addrSet) continue;

        PlayerStatePacket statePkt;
        statePkt.playerId = player.id;
        statePkt.sequence = player.statePacketSequence++;
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

const Player* Game::getPlayer(uint32_t playerId) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(_playersMutex));
    for (const auto& player : _players) {
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
    return _status;
}

void Game::setStatus(GameStatus status)
{
    _status = status;
}

uint32_t Game::getHostId() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(_playersMutex));
    return _players.empty() ? 0 : _players.front().id;
}

const std::vector<Player>& Game::getPlayers() const
{
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

    std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
    for (auto& entity : _entities) {
        if (_gameTime > 60.0f && (entity.type == 2 || entity.type == 3)) {
            entity.velocityY = 5.0f * std::sin(_gameTime * 2.0f + entity.id);
        }
        if (entity.type == 10) { // Boss movement
            if (entity.x > 1500) {
                entity.velocityX = -2.0f;
                entity.velocityY = 0.0f;
            } else {
                entity.velocityX = 0.0f;
                    if (g_bossLevel[this] == 3) { // Boss Level 2 (Faster)
                        entity.velocityY = 8.0f * std::sin(_gameTime * 4.0f);
                    } else { // Boss Level 1
                        entity.velocityY = 3.0f * std::sin(_gameTime);
                    }
            }
        }
    }
}

void Game::sendGlobalStateSync(UDPServer& udpServer) {
    std::lock_guard<std::mutex> lock_entities(_entitiesMutex);

    size_t totalPacketSize = sizeof(GlobalStateSyncPacket) + (_entities.size() * sizeof(SyncedEntityState));

    if (totalPacketSize > MAX_UDP_PACKET_SIZE) {
        std::cerr << "Warning: Global state sync packet size (" << totalPacketSize
                  << ") exceeds MAX_UDP_PACKET_SIZE (" << MAX_UDP_PACKET_SIZE
                  << "). Fragmentation or reduction of data sent per packet is required." << std::endl;
    }

    std::vector<char> packetBuffer(totalPacketSize);
    GlobalStateSyncPacket header;
    header.entityCount = _entities.size();

    std::memcpy(packetBuffer.data(), &header, sizeof(GlobalStateSyncPacket));

    size_t offset = sizeof(GlobalStateSyncPacket);
    for (const auto& entity : _entities) {
        SyncedEntityState state = {entity.id, entity.type, entity.x, entity.y};
        std::memcpy(packetBuffer.data() + offset, &state, sizeof(SyncedEntityState));
        offset += sizeof(SyncedEntityState);
    }

    std::lock_guard<std::mutex> lock_players(_playersMutex);
    for (const auto& destPlayer : _players) {
        if (destPlayer.addrSet) {
            udpServer.queueMessage(packetBuffer.data(), packetBuffer.size(), destPlayer.udpAddr);
        }
    }
}

void Game::update(UDPServer& udpServer) {
    if (_status != GameStatus::PLAYING)
        return;

    updateEntities(udpServer);
    handleCollision(udpServer);
    broadcastGameState(udpServer);
    updateGameLevel(0.016f);

    bool spawnBoss = false;
    int bossMaxHP = 1000;

    if (g_bossLevel[this] == 0 && _gameTime > 30.0f) {
        g_bossLevel[this] = 1;
        g_bossHP[this] = 1000;
        bossMaxHP = 1000;
        spawnBoss = true;
        std::cout << "[Game] Boss Level 1 Spawned!" << std::endl;
    } else if (g_bossLevel[this] == 2 && _gameTime > g_bossDeathTime[this] + 30.0f) { // Spawn Boss 2 (30s after death)
        g_bossLevel[this] = 3;
        g_bossHP[this] = 2000;
        bossMaxHP = 2000;
        spawnBoss = true;
        std::cout << "[Game] Boss Level 2 Spawned!" << std::endl;
    }

    if (spawnBoss) {
        std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
        uint32_t entityId = _nextEntityId++;
        // Spawn Boss: Type 10
        _entities.push_back({entityId, 10, 1600.0f, 400.0f, -2.0f, 0.0f, 296, 88});

        EntitySpawnPacket spawnPkt;
        spawnPkt.entityId = entityId;
        spawnPkt.entityType = 10;
        spawnPkt.x = 1600.0f;
        spawnPkt.y = 400.0f;

        BossStatePacket bossPkt;
        bossPkt.hp = g_bossHP[this];
        bossPkt.maxHp = bossMaxHP;

        std::lock_guard<std::mutex> lock_players(_playersMutex);
        for (const auto& destPlayer : _players) {
            if (destPlayer.addrSet) {
                udpServer.queueMessage(spawnPkt, destPlayer.udpAddr);
                udpServer.queueMessage(bossPkt, destPlayer.udpAddr);
            }
        }
    }

    // Boss Shooting Logic
    if (g_bossLevel[this] == 1 || g_bossLevel[this] == 3) {
        bool bossExists = false;
        float bossX = 0;
        float bossY = 0;
        
        {
            std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
            for (const auto& entity : _entities) {
                if (entity.type == 10) {
                    bossExists = true;
                    bossX = entity.x;
                    bossY = entity.y;
                    break;
                }
            }
        }

        float shootInterval = (g_bossLevel[this] == 3) ? 1.0f : 1.5f;

        if (bossExists && (_gameTime - g_lastBossShootTime[this] > shootInterval)) {
            g_lastBossShootTime[this] = _gameTime;
            std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
            std::lock_guard<std::mutex> lock_players(_playersMutex);

            std::vector<float> vyOffsets;
            if (g_bossLevel[this] == 3) vyOffsets = {-5.0f, 0.0f, 5.0f}; // Triple shot
            else vyOffsets = {0.0f}; // Single shot

            for (float vy : vyOffsets) {
                uint32_t projId = _nextEntityId++;
                _entities.push_back({projId, 11, bossX, bossY + 80, -15.0f, vy, 30, 30});

                EntitySpawnPacket spawnPkt;
                spawnPkt.entityId = projId;
                spawnPkt.entityType = 11;
                spawnPkt.x = bossX;
                spawnPkt.y = bossY + 80;

                for (const auto& destPlayer : _players) {
                    if (destPlayer.addrSet) udpServer.queueMessage(spawnPkt, destPlayer.udpAddr);
                }
            }
        }
    }

    if (std::chrono::steady_clock::now() - _lastEnemySpawnTime > std::chrono::seconds(2)) {
        createEnemy(udpServer);
        _lastEnemySpawnTime = std::chrono::steady_clock::now();
    }

    if (std::chrono::steady_clock::now() - _lastGlobalSyncTime >= GLOBAL_SYNC_INTERVAL) {
        sendGlobalStateSync(udpServer);
        _lastGlobalSyncTime = std::chrono::steady_clock::now();
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

void Game::handleCollision(UDPServer &udpServer) {
    std::lock_guard<std::mutex> lock_entities(_entitiesMutex);
    std::lock_guard<std::mutex> lock_players(_playersMutex);

    for (auto& projectile : _entities) {
        if (projectile.type != 1 && projectile.type != 4) continue;
        for (auto& enemy : _entities) {
            if (enemy.type != 2 && enemy.type != 3 && enemy.type != 10) continue;
            if (projectile.is_collide || enemy.is_collide) continue;
            if (checkCollision(projectile.x, projectile.y, projectile.width, projectile.height, enemy.x, enemy.y, enemy.width, enemy.height)) {
                projectile.is_collide = true;
                
                if (enemy.type == 10) { // Boss Logic
                    int damage = (projectile.type == 4) ? 50 : 10;
                    g_bossHP[this] -= damage;

                    BossStatePacket bossPkt;
                    bossPkt.hp = g_bossHP[this];
                    bossPkt.maxHp = (g_bossLevel[this] == 3) ? 2000 : 1000;

                    for (const auto& destPlayer : _players) {
                        if (destPlayer.addrSet) udpServer.queueMessage(bossPkt, destPlayer.udpAddr);
                    }

                    if (g_bossHP[this] <= 0) {
                        enemy.is_collide = true;
                        if (g_bossLevel[this] == 1) {
                            g_bossLevel[this] = 2; // Start cooldown for Boss 2
                            g_bossDeathTime[this] = _gameTime;
                            std::cout << "[Game] Boss 1 Defeated. Waiting for Boss 2..." << std::endl;
                        } else if (g_bossLevel[this] == 3) {
                            g_bossLevel[this] = 4; // Victory
                            std::cout << "[Game] Boss 2 Defeated. Victory!" << std::endl;
                        }
                    }
                } else {
                    enemy.is_collide = true;
                }
            }
        }
    }

    for (auto& player : _players) {
        for (auto& enemy : _entities) {
            if (enemy.type != 2 && enemy.type != 3 && enemy.type != 11)
                continue;
            if (enemy.is_collide)
                continue;

            if (checkCollision(player.x, player.y, 60, 30, enemy.x, enemy.y, enemy.width, enemy.height)) {
                enemy.is_collide = true;
            }
        }
    }
}

void Game::kickPlayer(uint32_t playerId, UDPServer& udpServer) {
    std::lock_guard<std::mutex> lock(_playersMutex);

    // Notify the kicked player
    for (const auto& player : _players) {
        if (player.id == playerId) {
            if (player.addrSet) {
                YouHaveBeenKickedPacket kickPkt;
                udpServer.queueMessage(kickPkt, player.udpAddr);
            }
            break;
        }
    }

    // Remove the player
    auto it = std::remove_if(_players.begin(), _players.end(),
                             [playerId](const Player& player) {
                                return player.id == playerId;
                             });

    if (it != _players.end()) {
        _players.erase(it, _players.end());
        std::cout << "[Game] Player " << playerId << " kicked." << std::endl;
    }

    // Notify remaining players
    for (const auto& destPlayer : _players) {
        if (destPlayer.addrSet) {
            PlayerDisconnectPacket disconnectPkt;
            disconnectPkt.playerId = playerId;
            udpServer.queueMessage(disconnectPkt, destPlayer.udpAddr);
        }
    }
}