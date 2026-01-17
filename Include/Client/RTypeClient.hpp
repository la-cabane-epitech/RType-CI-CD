/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** RTypeClient.hpp
*/

#ifndef RTYPECLIENT_HPP_
#define RTYPECLIENT_HPP_

#include "./UDPClient.hpp"
#include "./TCPClient.hpp"
#include "GameState.hpp"
#include "Renderer.hpp"
#include "Protocole/ProtocoleTCP.hpp"
#include <string>
#include <iostream>
#include "Clock.hpp"
#include <deque>

/**
 * @enum InGameStatus
 * @brief Represents the client's state while in the game phase.
 */
enum class InGameStatus {
    PLAYING,
    PAUSED,
    OPTIONS,
    QUITTING,
    KICKED
};

/**
 * @enum RTypeClientStatus
 * @brief Represents the final status of the RTypeClient after its run loop exits.
 */
enum class RTypeClientStatus {
    RUNNING,  // Should not be returned, internal use
    QUITTING, // User quit normally (ESC, closing window)
    KICKED    // Connection to server was lost
};

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
     * @param tcpClient Reference to the active TCP client for connection monitoring.
     * @param connectResponse The response received from the TCP handshake containing initial config.
     * @param keybinds The map of actions to key codes.
     */
    RTypeClient(const std::string& serverIp, TCPClient& tcpClient, const ConnectResponse& connectResponse, const std::map<std::string, int>& keybinds);

    /**
     * @brief Applies a player input packet to the local state (prediction).
     * @param packet The input packet to apply.
     */
    void applyInput(const PlayerInputPacket& packet);

    /**
     * @brief Updates the client state for a single frame (input, network processing).
     * @return The current status of the client (RUNNING, KICKED, QUITTING).
     */
    RTypeClientStatus updateFrame();

    /**
     * @brief Draws the client state for a single frame.
     */
    void drawFrame();

private:
    /**
     * @brief Captures and processes local player input.
     */
    void handleInput();

    /**
     * @brief Processes incoming network messages from the server.
     */
    void processNetworkMessages();

    TCPClient& _tcpClient; /**< TCP client for monitoring connection status */
    UDPClient _udpClient; /**< UDP client for real-time communication */
    GameState _gameState; /**< Current state of the game */
    Renderer _renderer;   /**< Renderer instance */
    uint32_t _tick = 0;   /**< Current game tick */
    Clock _clock;         /**< Clock for timing the game loop */
    std::map<std::string, int> _keybinds; /**< Map of actions to key codes */
    InGameStatus _status = InGameStatus::PLAYING; /**< Current status of the in-game client */

    std::deque<PlayerInputPacket> _pendingInputs; /**< Queue of inputs sent but not yet acknowledged */

    uint32_t _lastPingTime = 0; /**< Timestamp of the last ping sent */
    static constexpr uint32_t PING_INTERVAL_MS = 1000; /**< Interval between pings in milliseconds */

    bool _isChatActive = false;
    std::string _chatInput;
    std::vector<std::string> _chatHistory;
};

#endif // RTYPECLIENT_HPP_