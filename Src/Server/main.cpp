/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/

#include <fstream>
#include <iostream>
#include <thread>

// #include "Server/Network.hpp"
#include "Exception.hpp"
#include "Server/Game.hpp"
#include "Server/TCPServer.hpp"
#include "Server/UDPServer.hpp"
#include "Clock.hpp"

int main(void)
{
    try {
        Clock clock;
        Game game;
        TCPServer tcpServer(4242, game, clock);
        UDPServer udpServer(5252, game, clock);

        tcpServer.start();
        udpServer.start();

        auto lastEnemySpawnTime = std::chrono::steady_clock::now();

        while (true) {
            game.broadcastGameState(udpServer);
            game.updateEntities(udpServer);
            game.handleCollision();
            if (std::chrono::steady_clock::now() - lastEnemySpawnTime > std::chrono::seconds(2)) {
                game.createEnemy(udpServer);
                lastEnemySpawnTime = std::chrono::steady_clock::now();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    } catch (const RType::Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}