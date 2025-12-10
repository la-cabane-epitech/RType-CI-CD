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
        while (true) {
            game.broadcastGameState(udpServer);
            game.updateEntities(udpServer);
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60Hz
        }
    } catch (const RType::Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}