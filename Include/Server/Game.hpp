/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Game.hpp
*/

#ifndef GAME_HPP_
#define GAME_HPP_

#include <cstdint>
#include <vector>
#include <mutex>
#include <arpa/inet.h>
#include <optional>
#include <iostream>

#include "Protocole/ProtocoleUDP.hpp"
#include "../../ECS/include/registry.hpp"
#include "../../ECS/include/Component.hpp"

struct Network {
    sockaddr_in udpAddr;
    bool addrSet = false;
};

class Game {
public:
    Game() {
        _registry.register_component<Position>();
        _registry.register_component<Velocity>();
        _registry.register_component<Network>();
    }

    void addPlayer(uint32_t playerId) {
        std::lock_guard<std::mutex> lock(_gameMutex);
        Entity playerEntity = playerId; // Use playerId as entity ID for simplicity
        _registry.add_component<Position>(playerEntity, {400.0f, 225.0f});
        _registry.add_component<Velocity>(playerEntity, {0.0f, 0.0f});
        _registry.add_component<Network>(playerEntity, {});
        std::cout << "[Game] Player " << playerId << " entity created." << std::endl;
    }

    void updatePlayer(uint32_t playerId, uint8_t inputs, const sockaddr_in& udpAddr) {
        std::lock_guard<std::mutex> lock(_gameMutex);
        auto& velocities = _registry.get_components<Velocity>();
        auto& networks = _registry.get_components<Network>();

        if (playerId < networks.size() && networks[playerId] && !networks[playerId]->addrSet) {
            networks[playerId]->udpAddr = udpAddr;
            networks[playerId]->addrSet = true;
            std::cout << "[Game] Player " << playerId << " UDP address set." << std::endl;
        }

        if (playerId < velocities.size() && velocities[playerId]) {
            velocities[playerId]->vx = ((inputs & RIGHT) ? 5.0f : 0.0f) - ((inputs & LEFT) ? 5.0f : 0.0f);
            velocities[playerId]->vy = ((inputs & DOWN) ? 5.0f : 0.0f) - ((inputs & UP) ? 5.0f : 0.0f);
        }
    }

private:
    Registry _registry;
    std::mutex _gameMutex;
};


#endif /* !GAME_HPP_ */