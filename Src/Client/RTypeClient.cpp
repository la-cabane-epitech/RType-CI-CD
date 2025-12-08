/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** RTypeClient.cpp
*/

#include "Client/Ray.hpp"
#include "Client/RTypeClient.hpp"
#include "Protocole/ProtocoleUDP.hpp"

RTypeClient::RTypeClient(const std::string& serverIp, const ConnectResponse& connectResponse)
        : _udpClient(serverIp, connectResponse.udpPort), _renderer(_gameState)
{
    _gameState.myPlayerId = connectResponse.playerId;
    // Placement du joueur au centre de l'écran au démarrage
    _gameState.players[connectResponse.playerId] = { 800.0f / 2.0f, 450.0f / 2.0f };
}

void RTypeClient::run()
{
    PlayerInputPacket packet{};
    _udpClient.sendMessage(packet);
    update();

    while (!WindowShouldClose()) {
        handleInput();
        update();
        render();
    }
}

void RTypeClient::handleInput()
{
    PlayerInputPacket packet{};

    packet.type = UDPMessageType::PLAYER_INPUT;
    packet.playerId = _gameState.myPlayerId;
    packet.tick = _tick++;
    packet.inputs = 0;

    if (IsKeyDown(KEY_UP))    packet.inputs |= UP;
    if (IsKeyDown(KEY_DOWN))  packet.inputs |= DOWN;
    if (IsKeyDown(KEY_LEFT))  packet.inputs |= LEFT;
    if (IsKeyDown(KEY_RIGHT)) packet.inputs |= RIGHT;
    if (IsKeyDown(KEY_SPACE)) packet.inputs |= SHOOT;

    if (packet.inputs != 0) {
        _udpClient.sendMessage(packet);
    }
}

void RTypeClient::update()
{
    while (auto received = _udpClient.receiveMessage<1024>()) {
        const auto& data = *received;
        uint8_t type = data[0];

        if (type == UDPMessageType::PLAYER_STATE && data.size() >= sizeof(PlayerStatePacket)) {
            const auto* statePkt = reinterpret_cast<const PlayerStatePacket*>(data.data());
            _gameState.players[statePkt->playerId] = {statePkt->x, statePkt->y};
        }
    }
}

void RTypeClient::render()
{
    _renderer.draw();
}