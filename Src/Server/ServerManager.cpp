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
      _tcpServer(4242, _rooms, _roomsMutex, _clock),
      _udpServer(5252, _rooms, _roomsMutex, _clock),
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
        // Ce n'est pas une manière propre de quitter, car le thread peut être bloqué sur std::cin.
        // Pour un simple shell de débogage, c'est une limitation connue.
        // Une solution plus robuste impliquerait des I/O non bloquantes pour le shell.
        std::cout << "[ServerManager] Waiting for shell thread to exit. Press Enter to unblock." << std::endl;
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
                std::lock_guard<std::mutex> lock(_roomsMutex);
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
        std::lock_guard<std::mutex> lock(_roomsMutex);
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
        int newId = _tcpServer.createRoom();
        std::cout << "Room " << newId << " created." << std::endl;
    } else if (cmd == "delete") {
        int roomId;
        if (!(ss >> roomId)) {
            std::cout << "Usage: delete <room_id>" << std::endl;
            return;
        }
        std::lock_guard<std::mutex> lock(_roomsMutex);
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
            std::lock_guard<std::mutex> lock(_roomsMutex);
            for (auto const& [id, game] : _rooms) {
                if (game && game->getPlayer(playerId)) {
                    std::cout << "Player " << playerId << " found in room " << id << ". Removing from game instance." << std::endl;
                    // Notifie les autres joueurs et retire le joueur de la logique du jeu
                    game->disconnectPlayer(playerId, _udpServer);
                    playerFoundAndRemoved = true;
                    break; // Un joueur ne peut être que dans une seule salle
                }
            }
        }

        if (!playerFoundAndRemoved) {
            std::cout << "Player " << playerId << " not found in any active game. Will still attempt to close socket." << std::endl;
        }

        // Ferme la connexion TCP du joueur, ce qui le déconnectera côté client
        _tcpServer.kickPlayer(playerId);
        std::cout << "Kick signal sent for player " << playerId << "." << std::endl;
    } else {
        std::cout << "Unknown command: " << cmd << ". Type 'help' for a list of commands." << std::endl;
    }
}