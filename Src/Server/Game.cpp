/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Game
*/

#include "Server/Game.hpp"
#include "Server/UDPServer.hpp"

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