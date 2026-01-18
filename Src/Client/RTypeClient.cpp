/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** RTypeClient.cpp
*/

#include "Client/Ray.hpp"
#include "Client/RTypeClient.hpp"
#include "Protocole/ProtocoleUDP.hpp"

RTypeClient::RTypeClient(const std::string& serverIp, TCPClient& tcpClient, const ConnectResponse& connectResponse, const std::map<std::string, int>& keybinds)
    : _udpClient(serverIp, connectResponse.udpPort),
        _tcpClient(tcpClient),
        _renderer(_gameState),
        _tick(connectResponse.serverTimeMs),
        _clock(),
        _keybinds(keybinds),
        _score(0),
        _lastScoreIncreaseTime(0)
{
    _gameState.myPlayerId = connectResponse.playerId;

    PlayerInputPacket packet{};
    packet.playerId = _gameState.myPlayerId;
    _udpClient.sendMessage(packet);
}

void RTypeClient::tick()
{
    switch (_status) {
        case InGameStatus::PLAYING:
            if (IsKeyPressed(KEY_ESCAPE)) {
                _status = InGameStatus::PAUSED;
            }

            if (_clock.getElapsedTimeMs() - _lastScoreIncreaseTime >= 1000) {
                _score += 5;
                _lastScoreIncreaseTime = _clock.getElapsedTimeMs();
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
        default:
            break;
    }

    _renderer.draw(_keybinds);
    _renderer.drawChat(_chatHistory, _chatInput, _isChatActive);
    
    const char* scoreText = TextFormat("SCORE: %06d", _score);
    int textWidth = MeasureText(scoreText, 30);
    DrawText(scoreText, GetScreenWidth() - textWidth - 20, 20, 30, RAYWHITE);

    if (_status == InGameStatus::PAUSED) {
        PauseMenuChoice choice = _renderer.drawPauseMenu();
        if (choice == PauseMenuChoice::OPTIONS) _status = InGameStatus::OPTIONS;
        if (choice == PauseMenuChoice::QUIT) {
            _status = InGameStatus::QUITTING;
            PlayerDisconnectPacket disconnectPkt;
            disconnectPkt.type = UDPMessageType::PLAYER_DISCONNECT;
            disconnectPkt.playerId = _gameState.myPlayerId;
            _udpClient.sendMessage(disconnectPkt);
        }
    } else if (_status == InGameStatus::OPTIONS) {
        if (_renderer.drawOptionsMenu(_keybinds)) {
            _status = InGameStatus::PAUSED;
        }
    }
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
    if (IsKeyPressed(KEY_TAB)) {
        _isChatActive = !_isChatActive;
        return;
    }

    if (_isChatActive) {
        if (IsKeyPressed(KEY_ENTER)) {
            if (!_chatInput.empty()) {
                _tcpClient.sendChatMessage(_chatInput);
                _chatHistory.push_back("Me: " + _chatInput);
                _chatInput.clear();
            }
            _isChatActive = false;
            return;
        }

        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125) _chatInput += (char)key;
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !_chatInput.empty()) _chatInput.pop_back();
        return;
    }

    PlayerInputPacket packet{};

    packet.type = UDPMessageType::PLAYER_INPUT;
    packet.playerId = _gameState.myPlayerId;
    packet.tick = _tick++;
    packet.inputs = 0;

    if (IsKeyDown(_keybinds.at("UP")))    packet.inputs |= UP;
    if (IsKeyDown(_keybinds.at("DOWN")))  packet.inputs |= DOWN;
    if (IsKeyDown(_keybinds.at("LEFT")))  packet.inputs |= LEFT;
    if (IsKeyDown(_keybinds.at("RIGHT"))) packet.inputs |= RIGHT;

    static uint32_t chargeStart = 0;
    static bool isCharging = false;

    if (IsKeyPressed(KEY_SPACE)) {
        if (!isCharging) {
            chargeStart = _clock.getElapsedTimeMs();
            isCharging = true;
        }
    } else if (IsKeyReleased(KEY_SPACE)) {
        if (isCharging) {
            if (_clock.getElapsedTimeMs() - chargeStart > 500)
            packet.inputs |= CHARGE_SHOOT;
            else
            packet.inputs |= SHOOT;
            isCharging = false;
        }
    }

    if (packet.inputs != 0) {
        applyInput(packet);
        _pendingInputs.push_back(packet);
    }
    _udpClient.sendMessage(packet);
}

void RTypeClient::update()
{
    auto newMessages = _tcpClient.receiveChatMessages();
    _chatHistory.insert(_chatHistory.end(), newMessages.begin(), newMessages.end());

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
            
            if (_gameState.entities.count(destroyPkt->entityId)) {
                uint16_t type = _gameState.entities[destroyPkt->entityId].type;
                if (type == 2) _score += 50; 
                else if (type == 3) _score += 100;
                _renderer.addExplosion(_gameState.entities[destroyPkt->entityId].x, _gameState.entities[destroyPkt->entityId].y);
            }
            _gameState.entities.erase(destroyPkt->entityId);
        }

        if (type == UDPMessageType::PLAYER_DISCONNECT && data.size() >= sizeof(PlayerDisconnectPacket)) {
            const auto* disconnectPkt = reinterpret_cast<const PlayerDisconnectPacket*>(data.data());
            if (_gameState.players.count(disconnectPkt->playerId)) {
                _renderer.addExplosion(_gameState.players[disconnectPkt->playerId].x, _gameState.players[disconnectPkt->playerId].y);
            }
            _gameState.players.erase(disconnectPkt->playerId);
            std::cout << "[Game] Player " << disconnectPkt->playerId << " disconnected." << std::endl;
        }

        if (type == UDPMessageType::PONG && data.size() >= sizeof(PongPacket)) {
            const auto* pongPkt = reinterpret_cast<const PongPacket*>(data.data());
            uint32_t currentTime = _clock.getElapsedTimeMs();
            _gameState.rtt = currentTime - pongPkt->timestamp;
        }

        if (type == UDPMessageType::GLOBAL_STATE_SYNC && data.size() >= sizeof(GlobalStateSyncPacket)) {
            const auto* syncPkt = reinterpret_cast<const GlobalStateSyncPacket*>(data.data());
            size_t offset = sizeof(GlobalStateSyncPacket);
            _gameState.entities.clear();

            for (uint32_t i = 0; i < syncPkt->entityCount; ++i) {
                if (offset + sizeof(SyncedEntityState) <= data.size()) {
                    const auto* entityState = reinterpret_cast<const SyncedEntityState*>(data.data() + offset);
                    _gameState.entities[entityState->entityId] = {entityState->x, entityState->y, entityState->entityType};
                    offset += sizeof(SyncedEntityState);
                } else {
                    std::cerr << "Malformed GLOBAL_STATE_SYNC packet: not enough data for entity " << i << std::endl;
                    break;
                }
            }
        }

        if (type == UDPMessageType::YOU_HAVE_BEEN_KICKED) {
            std::cout << "[Game] You have been kicked." << std::endl;
            _status = InGameStatus::KICKED;
        }
    }
}
