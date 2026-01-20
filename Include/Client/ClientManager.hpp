/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ClientManager
*/

#ifndef CLIENTMANAGER_HPP_
#define CLIENTMANAGER_HPP_

#include <string>
#include <memory>
#include <optional>
#include "Network/TCP/TCPClient.hpp"
#include "Client/RTypeClient.hpp"
#include "Client/Renderer.hpp"
#include "Client/ConfigManager.hpp"

/**
 * @file ClientManager.hpp
 * @brief Header file for the ClientManager class.
 *
 * This file defines the ClientManager class, which acts as the main controller
 * for the client-side application. It manages the state machine, network connections,
 * and transitions between different menus and the game loop.
 */

/**
 * @enum ClientState
 * @brief Represents the various states of the client application.
 */
enum class ClientState {
    USERNAME_INPUT, /**< State for entering the username. */
    MAIN_MENU,      /**< Main menu state. */
    OPTIONS,        /**< Options menu state. */
    ROOM_SELECTION, /**< Room selection menu state. */
    LOBBY,          /**< Lobby state waiting for game start. */
    IN_GAME,        /**< Active game state. */
    KICKED,         /**< State when kicked from the server. */
    EXITING         /**< State indicating the application is closing. */
};

/**
 * @class ClientManager
 * @brief Manages the overall lifecycle and state of the client application.
 *
 * The ClientManager is responsible for initializing the window, handling the main loop,
 * managing transitions between different application states (menus, game, etc.),
 * and maintaining the TCP connection to the server.
 */
class ClientManager {
    public:
        /**
         * @brief Construct a new Client Manager object.
         *
         * Initializes the client, validates the server IP, and sets up the initial state.
         *
         * @param serverIp The IP address of the server to connect to.
         * @throws std::runtime_error If the server IP is invalid.
         */
        ClientManager(const std::string& serverIp);

        /**
         * @brief Destroy the Client Manager object.
         *
         * Cleans up resources and closes the window.
         */
        ~ClientManager();

        /**
         * @brief Runs the main application loop.
         *
         * This method contains the main while loop that processes input, updates logic,
         * and renders the current state until the application exits.
         */
        void run();

    private:
        std::string _serverIp; /**< The IP address of the server. */
        Config _config; /**< The client configuration (username, keybinds). */
        GameState _dummyState; /**< A dummy game state used for menu rendering. */
        std::unique_ptr<Renderer> _renderer; /**< The renderer instance. */
        ClientState _currentState; /**< The current state of the application. */
        TCPClient _tcpClient; /**< The TCP client for server communication. */

        ConnectResponse _connectRes; /**< Response from the server after connection. */
        std::vector<RoomInfo> _rooms; /**< List of available rooms. */
        double _lastRoomUpdate; /**< Timestamp of the last room list update. */
        LobbyState _lobbyState; /**< Current state of the lobby. */
        double _lastLobbyUpdate; /**< Timestamp of the last lobby state update. */
        bool _connected; /**< Flag indicating if connected to the server. */

        bool _createRoomInitiated; /**< Flag indicating if a room creation request is in progress. */
        bool _joinRoomInitiated; /**< Flag indicating if a join room request is in progress. */
        std::optional<int> _createRoomResponse; /**< Stores the result of a create room request. */
        std::optional<bool> _joinRoomResponse; /**< Stores the result of a join room request. */
        int _roomToJoin; /**< ID of the room currently being joined. */

        std::unique_ptr<RTypeClient> _gameInstance; /**< The active game instance when in IN_GAME state. */

        /**
         * @brief Validates the provided server IP address.
         * @throws std::runtime_error If the IP address is invalid.
         */
        void validateServerIp();
};

#endif /* !CLIENTMANAGER_HPP_ */