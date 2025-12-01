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

int main(void)
{
    try {
        Game game;
        TCPServer tcpServer(4242, game);
        UDPServer udpServer(5252, game);

        tcpServer.start();
        udpServer.start();
        while (true) {
            game.broadcastGameState(udpServer);
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60Hz
        }
    } catch (const RType::Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}