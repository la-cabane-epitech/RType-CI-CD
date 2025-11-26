/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/

#include <fstream>
#include <iostream>
#include <thread>

#include "Exception.hpp"
#include "Server/TCPServer.hpp"

int main(void)
{
    try {
        TCPServer tcpServer(4242);

        tcpServer.start();
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const RType::Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}