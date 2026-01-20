/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ServerManager
*/

#include "Server/ServerManager.hpp"
#include <iostream>
#include <thread>
#include <sstream>
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
    std::cout << "[ServerManager] Stopping servers..." << std::endl;
    _tcpServer.stop();
    _udpServer.stop();

    if (_shellThread.joinable()) {
        _shellThread.join();
    }
}

void ServerManager::run()
{
    try {
        _shellThread = std::thread(&ServerManager::shellLoop, this);
        _tcpServer.start();
        _udpServer.start();

        std::cout << "[ServerManager] Servers started. Entering game loop..." << std::endl;

        while (_running) {
            {
                std::lock_guard<std::mutex> lock(_serverMutex);
                for (auto& [id, game] : _rooms) {
                    if (game) {
                        game->update(_udpServer);
                    }
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
                            if (p->inputs & PRESSED) game->createPlayerShot(p->playerId, _udpServer);
                            if (p->inputs & HOLD) game->createPlayerChargedShot(p->playerId, _udpServer);
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

std::vector<Network::RoomSimpleInfo> ServerManager::onGetRooms() {
    std::lock_guard<std::mutex> lock(_serverMutex);
    std::vector<Network::RoomSimpleInfo> list;
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

void ServerManager::shellLoop()
{
    std::string line;
    std::cout << "Server shell started. Type 'help' for commands." << std::endl;
    while (_running) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            if (_running) _running = false;
            break;
        }
        if (!_running) break;
        if (!line.empty()) {
            processCommand(line);
        }
    }
    std::cout << "[Shell] Exiting." << std::endl;
}

void ServerManager::processCommand(const std::string& command)
{
    std::stringstream ss(command);
    std::string cmd;
    ss >> cmd;

    if (cmd == "exit") {
        std::cout << "Server shutting down..." << std::endl;
        _running = false;
    } else if (cmd == "help") {
        std::cout << "Available commands:\n"
                  << "  rooms                  - List all rooms\n"
                  << "  create                 - Create a new room\n"
                  << "  delete <room_id>       - Delete a room\n"
                  << "  kick <player_id>       - Kick a player from the server\n"
                  << "  exit                   - Shut down the server\n";
    } else if (cmd == "rooms") {
        std::lock_guard<std::mutex> lock(_serverMutex);
        if (_rooms.empty()) {
            std::cout << "No rooms available." << std::endl;
            return;
        }
        std::cout << "ID\tStatus\tPlayers\n" << "-----------------------\n";
        for (const auto& [id, game] : _rooms) {
            if (game) {
                std::string status = (game->getStatus() == GameStatus::PLAYING) ? "Playing" : "Lobby";
                std::cout << id << "\t" << status << "\t" << game->getPlayerCount() << "/4" << std::endl;
            }
        }
    } else if (cmd == "create") {
        int newId = onCreateRoom();
        std::cout << "Room " << newId << " created." << std::endl;
    } else if (cmd == "delete") {
        int roomId;
        if (!(ss >> roomId)) {
            std::cout << "Usage: delete <room_id>" << std::endl;
            return;
        }
        std::lock_guard<std::mutex> lock(_serverMutex);
        if (_rooms.erase(roomId)) {
            std::cout << "Room " << roomId << " deleted. Players inside will be disconnected." << std::endl;
        } else {
            std::cout << "Room " << roomId << " not found." << std::endl;
        }
    } else if (cmd == "kick") {
        uint32_t playerId;
        if (!(ss >> playerId)) {
            std::cout << "Usage: kick <player_id>" << std::endl;
            return;
        }

        bool playerFoundAndRemoved = false;
        {
            std::lock_guard<std::mutex> lock(_serverMutex);
            for (auto const& [id, game] : _rooms) {
                if (game && game->getPlayer(playerId)) {
                    game->kickPlayer(playerId, _udpServer);
                    playerFoundAndRemoved = true;
                    break;
                }
            }
        }

        if (!playerFoundAndRemoved) {
            std::cout << "Player " << playerId << " not found in any active game. Will still attempt to close socket." << std::endl;
        }

        _tcpServer.kickPlayer(playerId);
        std::cout << "Kick signal sent for player " << playerId << "." << std::endl;
    } else {
        std::cout << "Unknown command: " << cmd << ". Type 'help' for a list of commands." << std::endl;
    }
}