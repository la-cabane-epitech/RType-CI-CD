/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/

#include <iostream>
#include "Exception.hpp"
#include <memory>
#include "Server/ServerManager.hpp"

int main(void)
{
    try {
        auto serverManager = std::make_unique<ServerManager>();
        serverManager->run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}