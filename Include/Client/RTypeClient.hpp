/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** RTypeClient.hpp
*/

#ifndef RTYPECLIENT_HPP_
#define RTYPECLIENT_HPP_

#include "./UDPClient.hpp"
#include "GameState.hpp"
#include "Renderer.hpp"
#include "Protocole/ProtocoleTCP.hpp"
#include <string>
#include <iostream>
#include "Clock.hpp"
#include <deque>

class RTypeClient {
public:
    RTypeClient(const std::string& serverIp, const ConnectResponse& connectResponse);
    void applyInput(const PlayerInputPacket& packet);
    void run();

private:
    void handleInput();
    void update();
    void render();

    UDPClient _udpClient;
    GameState _gameState;
    Renderer _renderer;
    uint32_t _tick = 0;
    Clock _clock;

    // Historique des inputs pour la prédiction
    std::deque<PlayerInputPacket> _pendingInputs;

    uint32_t _lastPingTime = 0;
    static constexpr uint32_t PING_INTERVAL_MS = 1000; // Ping every second
};

#endif // RTYPECLIENT_HPP_