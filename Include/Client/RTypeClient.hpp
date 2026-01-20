/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** RTypeClient.hpp
*/

#ifndef RTYPECLIENT_HPP_
#define RTYPECLIENT_HPP_

#include "Network/UDP/UDPClient.hpp"
#include "Network/TCP/TCPClient.hpp"
#include "GameState.hpp"
#include "Renderer.hpp"
#include "Network/Protocole/ProtocoleTCP.hpp"
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
    KICKED,
    GAME_OVER
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

    void tick();

    InGameStatus getStatus() const { return _status; }

    int getScore() const { return _score; }
private:
    /**
     * @brief Captures and processes local player input.
     */
    void handleInput();

    /**
     * @brief Updates the game state based on network messages.
     */
    void update();

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

    int _score = 0;
    uint32_t _lastScoreIncreaseTime = 0;

    /**
     * @enum PacketStatus
     * @brief Represents the status of a packet in the event queue for loss calculation.
     */
    enum class PacketStatus {
        RECEIVED, ///< The packet was successfully received.
        LOST      ///< The packet was determined to be lost.
    };
    /**
     * @struct PacketEvent
     * @brief Stores an event (packet received or lost) with its timestamp.
     */
    struct PacketEvent {
        PacketStatus status; ///< The status of the packet.
        uint32_t timestamp;  ///< The time the event was recorded.
    };

    /// The duration of the sliding window for packet loss calculation, in seconds.
    static constexpr uint32_t PACKET_LOSS_WINDOW_SECONDS = 5;

    float _packetLossPercentage = 0.0f; ///< The calculated packet loss percentage over the sliding window.
    uint32_t _lastServerSeq = 0;        ///< The sequence number of the last received server state packet.
    bool _isFirstServerPacket = true;   ///< Flag to handle the first server packet differently for stats.
    std::deque<PacketEvent> _packetEvents; ///< A queue of recent packet events for the sliding window calculation.

    // --- END STATS ---

};

#endif // RTYPECLIENT_HPP_