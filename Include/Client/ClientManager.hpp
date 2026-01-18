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
#include "Client/TCPClient.hpp"
#include "Client/RTypeClient.hpp"
#include "Client/Renderer.hpp"
#include "Client/ConfigManager.hpp"

enum class ClientState {
    USERNAME_INPUT,
    MAIN_MENU,
    OPTIONS,
    ROOM_SELECTION,
    LOBBY,
    IN_GAME,
    KICKED,
    EXITING
};

class ClientManager {
    public:
        ClientManager(const std::string& serverIp);
        ~ClientManager();

        void run();

    private:
        std::string _serverIp;
        Config _config;
        GameState _dummyState;
        std::unique_ptr<Renderer> _renderer;
        ClientState _currentState;
        TCPClient _tcpClient;

        ConnectResponse _connectRes;
        std::vector<RoomInfo> _rooms;
        double _lastRoomUpdate;
        LobbyState _lobbyState;
        double _lastLobbyUpdate;
        bool _connected;

        bool _createRoomInitiated;
        bool _joinRoomInitiated;
        std::optional<int> _createRoomResponse;
        std::optional<bool> _joinRoomResponse;
        int _roomToJoin;

        std::unique_ptr<RTypeClient> _gameInstance;

        void validateServerIp();
};

#endif /* !CLIENTMANAGER_HPP_ */