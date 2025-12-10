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
    : _udpClient(serverIp, connectResponse.udpPort),
      _renderer(_gameState),
      _clock(connectResponse.clock)
{
    _gameState.myPlayerId = connectResponse.playerId;
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
    PlayerDisconnectPacket disconnectPacket{};
    disconnectPacket.playerId = _gameState.myPlayerId;
    _udpClient.sendMessage(disconnectPacket);
}

void RTypeClient::applyInput(const PlayerInputPacket& packet)
{
    // Applique le mouvement localement pour la prédiction
    if (!_gameState.players.count(_gameState.myPlayerId)) return;
    if (packet.inputs & UP)    _gameState.players[_gameState.myPlayerId].y -= 5;
    if (packet.inputs & DOWN)  _gameState.players[_gameState.myPlayerId].y += 5;
    if (packet.inputs & LEFT)  _gameState.players[_gameState.myPlayerId].x -= 5;
    if (packet.inputs & RIGHT) _gameState.players[_gameState.myPlayerId].x += 5;
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
    if (IsKeyPressed(KEY_SPACE)) packet.inputs |= SHOOT;

    if (packet.inputs != 0) {
        applyInput(packet); // Prédiction: on bouge tout de suite
        _udpClient.sendMessage(packet);
        _pendingInputs.push_back(packet); // On stocke pour la réconciliation
    }
}

void RTypeClient::update()
{
    uint32_t now = _clock.getElapsedTimeMs();
    if (now - _lastPingTime > PING_INTERVAL_MS) {
        PingPacket pingPkt;
        pingPkt.timestamp = now;
        _udpClient.sendMessage(pingPkt);
        _lastPingTime = now;
    }

    while (auto received = _udpClient.receiveMessage<1024>()) {
        const auto& data = *received;
        uint8_t type = data[0];

        if (type == UDPMessageType::PLAYER_STATE && data.size() >= sizeof(PlayerStatePacket)) {
            const auto* serverState = reinterpret_cast<const PlayerStatePacket*>(data.data());

            if (serverState->playerId == _gameState.myPlayerId) {
                // C'est une mise à jour de notre propre joueur (réconciliation)
                _gameState.players[serverState->playerId] = {serverState->x, serverState->y};

                // 1. On supprime les inputs qui ont été confirmés par le serveur
                while (!_pendingInputs.empty() && _pendingInputs.front().tick <= serverState->lastProcessedTick) {
                    _pendingInputs.pop_front();
                }

                // 2. On re-applique les inputs qui n'ont pas encore été traités par le serveur
                //    par-dessus l'état authoritaire du serveur.
                for (const auto& input : _pendingInputs) {
                    applyInput(input);
                }

            } else {
                // C'est une mise à jour d'un autre joueur (interpolation/extrapolation)
                // Pour l'instant, on applique directement la position.
                _gameState.players[serverState->playerId] = {serverState->x, serverState->y};
            }
        }

        if (type == UDPMessageType::ENTITY_SPAWN && data.size() >= sizeof(EntitySpawnPacket)) {
            const auto* spawnPkt = reinterpret_cast<const EntitySpawnPacket*>(data.data());
            _gameState.entities[spawnPkt->entityId] = {spawnPkt->x, spawnPkt->y};
        }

        if (type == UDPMessageType::ENTITY_UPDATE && data.size() >= sizeof(EntityUpdatePacket)) {
            const auto* updatePkt = reinterpret_cast<const EntityUpdatePacket*>(data.data());
            if (_gameState.entities.count(updatePkt->entityId)) {
                _gameState.entities[updatePkt->entityId] = {updatePkt->x, updatePkt->y};
            }
        }

        if (type == UDPMessageType::ENTITY_DESTROY && data.size() >= sizeof(EntityDestroyPacket)) {
            const auto* destroyPkt = reinterpret_cast<const EntityDestroyPacket*>(data.data());
            _gameState.entities.erase(destroyPkt->entityId);
        }

        if (type == UDPMessageType::PLAYER_DISCONNECT && data.size() >= sizeof(PlayerDisconnectPacket)) {
            const auto* disconnectPkt = reinterpret_cast<const PlayerDisconnectPacket*>(data.data());
            _gameState.players.erase(disconnectPkt->playerId);
            std::cout << "[Game] Player " << disconnectPkt->playerId << " disconnected." << std::endl;
        }

        if (type == UDPMessageType::PONG && data.size() >= sizeof(PongPacket)) {
            const auto* pongPkt = reinterpret_cast<const PongPacket*>(data.data());
            uint32_t currentTime = _clock.getElapsedTimeMs();
            _gameState.rtt = currentTime - pongPkt->timestamp;
        }
    }
}

void RTypeClient::render()
{
    _renderer.draw();
}
