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

/**
 * @class RTypeClient
 * @brief Main client class that orchestrates the game loop, networking, and rendering.
 */
class RTypeClient {
public:
    /**
     * @brief Construct a new RTypeClient object.
     *
     * @param serverIp The IP address of the server.
     * @param connectResponse The response received from the TCP handshake containing initial config.
     * @param keybinds The map of actions to key codes.
     */
    RTypeClient(const std::string& serverIp, const ConnectResponse& connectResponse, const std::map<std::string, int>& keybinds);

    /**
     * @brief Applies a player input packet to the local state (prediction).
     * @param packet The input packet to apply.
     */
    void applyInput(const PlayerInputPacket& packet);

    /**
     * @brief Starts the main game loop.
     */
    void run();

private:
    /**
     * @brief Captures and processes local player input.
     */
    void handleInput();

    /**
     * @brief Updates the game logic and processes network messages.
     */
    void update();

    /**
     * @brief Renders the current frame.
     */
    void render();

    UDPClient _udpClient; /**< UDP client for real-time communication */
    GameState _gameState; /**< Current state of the game */
    Renderer _renderer;   /**< Renderer instance */
    uint32_t _tick = 0;   /**< Current game tick */
    Clock _clock;         /**< Clock for timing the game loop */
    std::map<std::string, int> _keybinds; /**< Map of actions to key codes */

    std::deque<PlayerInputPacket> _pendingInputs; /**< Queue of inputs sent but not yet acknowledged */

    uint32_t _lastPingTime = 0; /**< Timestamp of the last ping sent */
    static constexpr uint32_t PING_INTERVAL_MS = 1000; /**< Interval between pings in milliseconds */
};

#endif // RTYPECLIENT_HPP_