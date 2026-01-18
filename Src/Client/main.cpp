/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/

#include <iostream>
#include <exception>
#include <memory>
#include "Client/ClientManager.hpp"

int main(int ac, char **av)
{
    try {
        if (ac != 2)
            throw std::runtime_error("Usage: " + std::string(av[0]) + " <server_ip>");
        auto clientManager = std::make_unique<ClientManager>(av[1]);
        clientManager->run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}