/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** RTypeClient.cpp
*/

#include "Client/Ray.hpp"
#include "Client/RTypeClient.hpp"
#include "Protocole/ProtocoleUDP.hpp"

RTypeClient::RTypeClient(const std::string& serverIp, const ConnectResponse& connectResponse, const std::map<std::string, int>& keybinds)
    : _udpClient(serverIp, connectResponse.udpPort),
      _renderer(_gameState),
      _clock(connectResponse.clock),
      _keybinds(keybinds)
{
    _gameState.myPlayerId = connectResponse.playerId;
}

void RTypeClient::run()
{
    PlayerInputPacket packet{};
    _udpClient.sendMessage(packet);
    update();

    while (!WindowShouldClose() && _status != InGameStatus::QUITTING) {
        switch (_status) {
            case InGameStatus::PLAYING:
                if (IsKeyPressed(KEY_ESCAPE)) {
                    _status = InGameStatus::PAUSED;
                }
                handleInput();
                update();
                break;
            case InGameStatus::PAUSED:
                if (IsKeyPressed(KEY_ESCAPE)) {
                    _status = InGameStatus::PLAYING;
                }
                break;
            case InGameStatus::OPTIONS:
                if (IsKeyPressed(KEY_ESCAPE)) {
                    _status = InGameStatus::PAUSED;
                }
                break;
            case InGameStatus::QUITTING:
                break;
        }

        BeginDrawing();
        _renderer.draw();

        if (_status == InGameStatus::PAUSED) {
            PauseMenuChoice choice = _renderer.drawPauseMenu();
            if (choice == PauseMenuChoice::OPTIONS) _status = InGameStatus::OPTIONS;
            if (choice == PauseMenuChoice::QUIT) _status = InGameStatus::QUITTING;
        } else if (_status == InGameStatus::OPTIONS) {
            if (_renderer.drawOptionsMenu(_keybinds)) {
                _status = InGameStatus::PAUSED;
            }
        }
        EndDrawing();
    }
    PlayerDisconnectPacket disconnectPacket{};
    disconnectPacket.playerId = _gameState.myPlayerId;
    _udpClient.sendMessage(disconnectPacket);
}

void RTypeClient::applyInput(const PlayerInputPacket& packet)
{
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

    if (IsKeyDown(_keybinds.at("UP")))    packet.inputs |= UP;
    if (IsKeyDown(_keybinds.at("DOWN")))  packet.inputs |= DOWN;
    if (IsKeyDown(_keybinds.at("LEFT")))  packet.inputs |= LEFT;
    if (IsKeyDown(_keybinds.at("RIGHT"))) packet.inputs |= RIGHT;
    if (IsKeyPressed(_keybinds.at("SHOOT"))) packet.inputs |= SHOOT;

    static uint32_t chargeStart = 0;
    static bool isCharging = false;

    if (IsKeyDown(KEY_SPACE)) {
        if (!isCharging) {
            chargeStart = _clock.getElapsedTimeMs();
            isCharging = true;
        }
    } else if (isCharging) {
        if (_clock.getElapsedTimeMs() - chargeStart > 500)
            packet.inputs |= CHARGE_SHOOT;
        else
            packet.inputs |= SHOOT;
        isCharging = false;
    }
    if (packet.inputs != 0) {
        applyInput(packet);
        _udpClient.sendMessage(packet);
        _pendingInputs.push_back(packet);
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
                _gameState.players[serverState->playerId] = {serverState->x, serverState->y};

                while (!_pendingInputs.empty() && _pendingInputs.front().tick <= serverState->lastProcessedTick) {
                    _pendingInputs.pop_front();
                }

                for (const auto& input : _pendingInputs) {
                    applyInput(input);
                }
            } else {
                _gameState.players[serverState->playerId] = {serverState->x, serverState->y};
            }
        }

        if (type == UDPMessageType::ENTITY_SPAWN && data.size() >= sizeof(EntitySpawnPacket)) {
            const auto* spawnPkt = reinterpret_cast<const EntitySpawnPacket*>(data.data());
            _gameState.entities[spawnPkt->entityId] = {spawnPkt->x, spawnPkt->y, spawnPkt->entityType};
        }

        if (type == UDPMessageType::ENTITY_UPDATE && data.size() >= sizeof(EntityUpdatePacket)) {
            const auto* updatePkt = reinterpret_cast<const EntityUpdatePacket*>(data.data());
            if (_gameState.entities.count(updatePkt->entityId)) {
                _gameState.entities[updatePkt->entityId].x = updatePkt->x;
                _gameState.entities[updatePkt->entityId].y = updatePkt->y;
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
