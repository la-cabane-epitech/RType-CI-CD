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
      _tcpServer(4242, this, _clock),
      _udpServer(5252, this, _clock),
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

void ServerManager::onMessageReceived(const char* data, size_t length, const sockaddr_in& clientAddr)
{
    if (length < 1) return;

    uint8_t type = *reinterpret_cast<const uint8_t*>(data);

    switch (type) {
        case PLAYER_INPUT:
            if (length == sizeof(PlayerInputPacket)) {
                const auto* p = reinterpret_cast<const PlayerInputPacket*>(data);
                for (auto& [id, game] : _rooms) {
                    if (game->getPlayer(p->playerId)) {
                        game->updatePlayerUdpAddr(p->playerId, clientAddr);
                        game->setPlayerLastProcessedTick(p->playerId, p->tick);

                        if (Player* player = game->getPlayer(p->playerId)) {
                            if (p->inputs & UP) player->y -= player->velocity;
                            if (p->inputs & DOWN) player->y += player->velocity;
                            if (p->inputs & LEFT) player->x -= player->velocity;
                            if (p->inputs & RIGHT) player->x += player->velocity;
                            if (p->inputs & SHOOT) game->createPlayerShot(p->playerId, _udpServer);
                            if (p->inputs & CHARGE_SHOOT) game->createPlayerChargedShot(p->playerId, _udpServer);
                        }
                        break;
                    }
                }
            }
            break;
        case PLAYER_DISCONNECT:
            if (length == sizeof(PlayerDisconnectPacket)) {
                const auto* p = reinterpret_cast<const PlayerDisconnectPacket*>(data);
                for (auto& [id, game] : _rooms) {
                    if (game->getPlayer(p->playerId)) {
                        game->disconnectPlayer(p->playerId, _udpServer);
                        break;
                    }
                }
            }
            break;
        case PING:
            if (length == sizeof(PingPacket)) {
                const auto* p = reinterpret_cast<const PingPacket*>(data);
                PongPacket pongPkt{ .type = PONG, .timestamp = p->timestamp };
                _udpServer.queueMessage(pongPkt, clientAddr);
            }
            break;
    }
}

int ServerManager::onCreateRoom() {
    std::lock_guard<std::mutex> lock(_serverMutex);
    int id = _nextRoomId++;
    _rooms[id] = std::make_shared<Game>();
    return id;
}

std::vector<RoomSimpleInfo> ServerManager::onGetRooms() {
    std::vector<RoomSimpleInfo> list;
    for (auto const& [id, game] : _rooms) {
        list.push_back({id, game->getPlayerCount()});
    }
    return list;
}

bool ServerManager::onJoinRoom(int roomId, uint32_t playerId, const std::string& username) {
    std::lock_guard<std::mutex> lock(_serverMutex);
    auto it = _rooms.find(roomId);
    if (it != _rooms.end() && it->second->getStatus() == GameStatus::LOBBY) {
        it->second->addPlayer(playerId, username.c_str());
        std::cout << "[ServerManager] Player " << username << " joined room " << roomId << std::endl;
        return true;
    }
    return false;
}

std::vector<uint32_t> ServerManager::onGetPlayersInRoom(int roomId) {
    std::lock_guard<std::mutex> lock(_serverMutex);
    std::vector<uint32_t> playerIds;
    auto it = _rooms.find(roomId);
    if (it != _rooms.end()) {
        for (const auto& player : it->second->getPlayers()) {
            playerIds.push_back(player.id);
        }
    }
    return playerIds;
}

bool ServerManager::isGameStarting(int roomId) {
    std::lock_guard<std::mutex> lock(_serverMutex);
    auto it = _rooms.find(roomId);
    if (it != _rooms.end()) {
        return it->second->getStatus() == GameStatus::PLAYING;
    }
    return false;
}

void ServerManager::onPlayerDisconnect(uint32_t playerId, int roomId) {
    std::lock_guard<std::mutex> lock(_serverMutex);
    if (_rooms.count(roomId)) {
        _rooms[roomId]->removePlayerFromLobby(playerId);
        std::cout << "[ServerManager] Player " << playerId << " left room " << roomId << std::endl;
    }
}

void ServerManager::onGetLobbyState(int roomId, uint32_t& hostId, std::vector<std::pair<uint32_t, std::string>>& players) {
    std::lock_guard<std::mutex> lock(_serverMutex);
    if (_rooms.count(roomId)) {
        auto game = _rooms[roomId];
        hostId = game->getHostId();
        for (const auto& p : game->getPlayers()) {
            players.push_back({p.id, p.username});
        }
    }
}

void ServerManager::onStartGame(int roomId, uint32_t playerId) {
    std::lock_guard<std::mutex> lock(_serverMutex);
    if (_rooms.count(roomId)) {
        if (playerId == _rooms[roomId]->getHostId()) {
            _rooms[roomId]->setStatus(GameStatus::PLAYING);
            std::cout << "[ServerManager] Room " << roomId << " starting game!" << std::endl;
        }
    }
}