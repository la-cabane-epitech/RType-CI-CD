/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** RTypeClient.hpp
*/

#ifndef RTYPECLIENT_HPP_
#define RTYPECLIENT_HPP_

#include "UDPClient.hpp"
#include "GameState.hpp"
#include "Renderer.hpp"
#include "Protocole/ProtocoleTCP.hpp"
#include <string>

class RTypeClient {
public:
    RTypeClient(const std::string& serverIp, const ConnectResponse& connectResponse);
    void run();

private:
    void handleInput();
    void update();
    void render();

    UDPClient _udpClient;
    GameState _gameState;
    Renderer _renderer;
    uint32_t _tick = 0;
};

#endif // RTYPECLIENT_HPP_