/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ServerManager
*/

#include "Server/ServerManager.hpp"
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>

ServerManager::ServerManager()
    : _clock(), _tcpServer(4242, _rooms, _clock, _roomsMutex),
      _udpServer(5252, _rooms, _clock, _roomsMutex),
      _running(true)
{
}

ServerManager::~ServerManager()
{
    _running = false;
    _tcpServer.stop();
    _udpServer.stop();
    if (_shellThread.joinable()) {
        // This relies on the 'exit' command or Ctrl+D to unblock std::cin
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
            std::vector<std::shared_ptr<Game>> gamesToUpdate;
            {
                std::lock_guard<std::mutex> lock(_roomsMutex);
                for (auto const& [id, game] : _rooms) {
                    gamesToUpdate.push_back(game);
                }
            }

            for (auto& game : gamesToUpdate) {
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

void ServerManager::shellLoop()
{
    std::string line;
    std::cout << std::endl << "[Shell] Server shell started. Type 'help' for commands." << std::endl;

    while (_running) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            if (_running) {
                _running = false;
                std::cout << "Input stream closed. Shutting down server..." << std::endl;
            }
            break;
        }
        if (line.empty()) {
            continue;
        }
        processCommand(line);
    }
    std::cout << "[Shell] Shell is shutting down." << std::endl;
}

void ServerManager::processCommand(const std::string& commandLine)
{
    std::stringstream ss(commandLine);
    std::string command;
    ss >> command;

    if (command == "help") {
        std::cout << "Available commands:\n"
                  << "  help                  - Show this help message\n"
                  << "  list_rooms            - List all game rooms\n"
                  << "  room_info <id>        - Show detailed information about a room\n"
                  << "  create_room           - Create a new game room\n"
                  << "  kick_player <room_id> <player_id> - Kick a player from a room\n"
                  << "  delete_room <id>      - Delete a game room\n"
                  << "  exit                  - Shut down the server" << std::endl;
    } else if (command == "list_rooms") {
        std::lock_guard<std::mutex> lock(_roomsMutex);
        if (_rooms.empty()) {
            std::cout << "No rooms available." << std::endl;
        } else {
            std::cout << "ID\tStatus\t\tPlayers" << std::endl;
            for (const auto& [id, game] : _rooms) {
                std::cout << id << "\t"
                          << (game->getStatus() == GameStatus::LOBBY ? "Lobby  " : "Playing") << "\t"
                          << game->getPlayerCount() << "/4" << std::endl;
            }
        }
    } else if (command == "room_info") {
        int roomId;
        if (!(ss >> roomId)) {
            std::cout << "Usage: room_info <id>" << std::endl;
            return;
        }
        std::lock_guard<std::mutex> lock(_roomsMutex);
        auto it = _rooms.find(roomId);
        if (it == _rooms.end()) {
            std::cout << "Room " << roomId << " not found." << std::endl;
        } else {
            auto game = it->second;
            std::cout << "Room ID: " << it->first << ", Status: " << (game->getStatus() == GameStatus::LOBBY ? "Lobby" : "Playing") << std::endl;
            auto players = game->getPlayers();
            std::cout << "Players (" << players.size() << "):" << std::endl;
            for (const auto& player : players) {
                std::cout << "  - Player " << player.id << " (" << player.username << ")" << std::endl;
            }
        }
    } else if (command == "create_room") {
        std::lock_guard<std::mutex> lock(_roomsMutex);
        int newRoomId = _tcpServer.createRoom();
        std::cout << "Room " << newRoomId << " created." << std::endl;
    } else if (command == "delete_room") {
        int roomId;
        if (!(ss >> roomId)) {
            std::cout << "Usage: delete_room <id>" << std::endl;
            return;
        }
        std::lock_guard<std::mutex> lock(_roomsMutex);
        if (_rooms.erase(roomId)) {
            std::cout << "Room " << roomId << " deleted." << std::endl;
        } else {
            std::cout << "Room " << roomId << " not found." << std::endl;
        }
    } else if (command == "kick_player") {
        int roomId;
        uint32_t playerId;
        if (!(ss >> roomId >> playerId)) {
            std::cout << "Usage: kick_player <room_id> <player_id>" << std::endl;
            return;
        }
        std::shared_ptr<Game> game;
        {
            std::lock_guard<std::mutex> lock(_roomsMutex);
            auto it = _rooms.find(roomId);
            if (it == _rooms.end()) {
                std::cout << "Room " << roomId << " not found." << std::endl;
                return;
            }
            game = it->second;
        }

        if (game->getPlayer(playerId)) {
            game->kickPlayer(playerId, _udpServer);
            std::cout << "Player " << playerId << " kicked from room " << roomId << "." << std::endl;
        } else {
            std::cout << "Player " << playerId << " not found in room " << roomId << "." << std::endl;
        }
    } else if (command == "exit") {
        std::cout << "Server shutting down..." << std::endl;
        _running = false;
    } else {
        std::cout << "Unknown command: " << command << ". Type 'help' for a list of commands." << std::endl;
    }
}