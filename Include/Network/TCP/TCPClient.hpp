/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPClient
*/

#ifndef TCPCLIENT_HPP_
#define TCPCLIENT_HPP_

#include <string>
#include <stdint.h>
#include <cstring>
#include <vector>
#include "Protocole/ProtocoleTCP.hpp"
#include "Client/Asio.hpp"

/**
 * @file TCPClient.hpp
 * @brief Header file for the TCPClient class.
 *
 * This file contains the definition of the TCPClient class, which is responsible
 * for handling TCP communications with the game server, including connection,
 * room management, and lobby state synchronization.
 */

/**
 * @struct LobbyState
 * @brief Represents the state of a game lobby.
 *
 * This structure holds the current status of a lobby, including whether the game
 * is starting, the host's identifier, and the list of players currently in the lobby.
 */
struct LobbyState {
    bool gameIsStarting = false; /**< Flag indicating if the game is about to start. */
    uint32_t hostId = 0; /**< The ID of the player who is the host of the lobby. */
    std::vector<LobbyPlayerInfo> players; /**< A list of players in the lobby. */
    bool disconnected = false; /**< Flag indicating if the connection was lost during the request. */
};

/**
 * @class TCPClient
 * @brief Handles the TCP connection for the initial handshake and lobby management with the server.
 *
 * The TCPClient class manages the TCP socket used for reliable communication with the server.
 * It handles operations such as connecting to the server, listing rooms, creating/joining rooms,
 * and synchronizing the lobby state. It supports non-blocking operations for room creation and joining.
 */
class TCPClient {
private:
    asio::io_context _io_context; /**< ASIO IO context for managing I/O operations. */
    asio::ip::tcp::socket _socket; /**< The TCP socket used for communication. */
    std::string _serverIp; /**< The IP address of the server. */
    uint16_t _port; /**< The TCP port of the server. */
    bool _socket_non_blocking_set = false; /**< Flag tracking if the socket has been set to non-blocking mode. */

    /**
     * @enum RequestState
     * @brief Internal state enum for managing non-blocking requests (Create/Join Room).
     */
    enum class RequestState {
        IDLE,           /**< No request is currently in progress. */
        SENT_REQUEST,   /**< The request has been sent to the server. */
        RECEIVED_TYPE,  /**< The response type header has been received. */
        RECEIVED_DATA   /**< The response data has been received. */
    };

    RequestState _createRoomState = RequestState::IDLE; /**< Current state of the create room request. */
    RequestState _joinRoomState = RequestState::IDLE; /**< Current state of the join room request. */
    uint8_t _createRoomPendingRespType = 0; /**< Stores the response type received for create room while waiting for data. */
    uint8_t _joinRoomPendingRespType = 0; /**< Stores the response type received for join room while waiting for data. */

public:
    /**
     * @brief Construct a new TCPClient object.
     *
     * @param serverIp The IP address of the server to connect to.
     * @param port The TCP port of the server.
     */
    TCPClient(const std::string& serverIp, uint16_t port);

    /**
     * @brief Destroy the TCPClient object.
     *
     * Closes the socket if it is open.
     */
    ~TCPClient();

    /**
     * @brief Establishes a connection to the server.
     *
     * Resolves the server address and attempts to connect the TCP socket.
     * Sets the socket to non-blocking mode after a successful connection.
     *
     * @return true if the connection is successful, false otherwise.
     */
    bool connectToServer();

    /**
     * @brief Closes the connection to the server.
     */
    void disconnect();

    /**
     * @brief Sends a connection request (handshake) to the server.
     *
     * Sends the player's username to the server and waits for a response containing
     * the assigned player ID and UDP port. This operation temporarily switches the
     * socket to blocking mode to ensure the handshake completes.
     *
     * @param username The username of the player.
     * @param outResponse Reference to a ConnectResponse structure to store the server's response.
     * @return true if the handshake was successful, false otherwise.
     */
    bool sendConnectRequest(const std::string& username, ConnectResponse& outResponse);

    /**
     * @brief Requests the list of available rooms from the server.
     *
     * Sends a request to list rooms and reads the response.
     *
     * @return A std::vector containing RoomInfo structures for each available room.
     */
    std::vector<RoomInfo> getRooms();

    /**
     * @brief Requests the server to create a new room (Non-blocking).
     *
     * This method implements a state machine to handle the creation request without blocking the main thread.
     * It should be called repeatedly until a value is returned.
     *
     * @return std::optional<int> containing:
     *         - The ID of the created room if successful.
     *         - -1 if an error occurred.
     *         - std::nullopt if the operation is still pending (waiting for server response).
     */
    std::optional<int> createRoom();

    /**
     * @brief Requests to join a specific room (Non-blocking).
     *
     * This method implements a state machine to handle the join request without blocking the main thread.
     * It should be called repeatedly until a value is returned.
     *
     * @param roomId The ID of the room to join.
     * @return std::optional<bool> containing:
     *         - true if the room was joined successfully.
     *         - false if the join failed.
     *         - std::nullopt if the operation is still pending (waiting for server response).
     */
    std::optional<bool> joinRoom(int roomId);

    /**
     * @brief Polls the server for the current state of the lobby.
     *
     * Sends a request to get the lobby state and processes pending packets to update
     * the local view of the lobby (players, host, game start status).
     *
     * @return A LobbyState structure containing the current lobby information.
     */
    LobbyState getLobbyState();

    /**
     * @brief Sends a request to the server to start the game.
     *
     * This command is typically sent by the host of the lobby to initiate the game session.
     */
    void sendStartGameRequest();

    /**
     * @brief Polls the TCP socket to check if the connection is still active.
     * @return true if the connection is alive, false if it has been closed.
     */
    bool checkConnection();
    /**
     * @brief Sends a chat message to the server.
     * @param message The content of the message.
     */
    void sendChatMessage(const std::string& message);

    /**
     * @brief Checks for incoming chat messages without blocking.
     * @return A vector of received messages.
     */
    std::vector<std::string> receiveChatMessages();

};

#endif // TCPCLIENT_HPP_
