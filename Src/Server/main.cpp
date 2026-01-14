/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/

#include <iostream>
#include "Exception.hpp"
#include "Server/ServerManager.hpp"

int main(void)
{
    try {
        ServerManager serverManager;
        serverManager.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    std::cout << "\nServer has stopped. Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}