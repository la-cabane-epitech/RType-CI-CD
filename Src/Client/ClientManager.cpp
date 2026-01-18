/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ClientManager
*/

#include "Client/ClientManager.hpp"
#include "Client/Ray.hpp"
#include <iostream>
#include <stdexcept>

ClientManager::ClientManager(const std::string& serverIp)
    : _serverIp(serverIp),
      _config(ConfigManager::loadConfig("config_file")),
      _dummyState(),
      _currentState(ClientState::USERNAME_INPUT),
      _tcpClient(serverIp, 4242),
      _lastRoomUpdate(0),
      _lastLobbyUpdate(0),
      _connected(false),
      _createRoomInitiated(false),
      _joinRoomInitiated(false),
      _roomToJoin(-1),
      _gameInstance(nullptr)
{
    validateServerIp();

    if (!_config.username.empty()) {
        _currentState = ClientState::MAIN_MENU;
    }

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "R-Type Client");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    _renderer = std::make_unique<Renderer>(_dummyState);
}

ClientManager::~ClientManager()
{
    _renderer.reset();
    CloseWindow();
}

void ClientManager::validateServerIp()
{
    try {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        resolver.resolve(_serverIp, "4242");
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid server IP address or unreachable host: " + _serverIp);
    }
}

void ClientManager::run()
{
    const std::string CONFIG_FILE = "config_file";

    while (_currentState != ClientState::EXITING && !WindowShouldClose()) {
        BeginDrawing();

        switch (_currentState) {
            case ClientState::USERNAME_INPUT: {
                if (_renderer->drawUsernameInput(_config.username)) {
                    ConfigManager::saveConfig(_config, CONFIG_FILE);
                    _currentState = ClientState::MAIN_MENU;
                }
                break;
            }
            case ClientState::MAIN_MENU: {
                MainMenuChoice choice = _renderer->drawMainMenu();
                if (choice == MainMenuChoice::START) {
                    _currentState = ClientState::ROOM_SELECTION;
                } else if (choice == MainMenuChoice::OPTIONS) {
                    _currentState = ClientState::OPTIONS;
                }
                break;
            }
            case ClientState::OPTIONS: {
                if (_renderer->drawOptionsMenu(_config.keybinds)) {
                    _currentState = ClientState::MAIN_MENU;
                    ConfigManager::saveConfig(_config, CONFIG_FILE);
                }
                break;
            }
            case ClientState::ROOM_SELECTION: {
                if (!_connected) {
                    if (!_tcpClient.connectToServer()) {
                        std::cerr << "Unable to connect to TCP server. Retrying...\n";
                        _currentState = ClientState::MAIN_MENU;
                        break;
                    }
                    if (!_tcpClient.sendConnectRequest(_config.username, _connectRes)) {
                        std::cerr << "TCP Handshake failed\n";
                        _currentState = ClientState::EXITING;
                        break;
                    }
                    std::cout << "Connected! PlayerId: " << _connectRes.playerId << ", UDP Port: " << _connectRes.udpPort << "\n";
                    _connected = true;
                }

                double now = GetTime();
                if (now - _lastRoomUpdate > 1.0 && !_createRoomInitiated && !_joinRoomInitiated) {
                    _rooms = _tcpClient.getRooms();
                    _lastRoomUpdate = now;
                }

                int action = _renderer->drawRoomMenu(_rooms);

                if (action == -2 && !_createRoomInitiated && !_joinRoomInitiated) {
                    _createRoomInitiated = true;
                    _createRoomResponse = _tcpClient.createRoom();
                }

                if (_createRoomInitiated) {
                    if (!_createRoomResponse.has_value()) {
                        _createRoomResponse = _tcpClient.createRoom();
                    }
                    if (_createRoomResponse.has_value()) {
                        int newRoomId = _createRoomResponse.value();
                        if (newRoomId >= 0) {
                            _joinRoomInitiated = true;
                            _roomToJoin = newRoomId;
                            _joinRoomResponse = _tcpClient.joinRoom(_roomToJoin);
                        }
                        _createRoomInitiated = false;
                        _createRoomResponse = std::nullopt;
                    }
                }

                if (action >= 0 && !_createRoomInitiated && !_joinRoomInitiated) {
                    _joinRoomInitiated = true;
                    _roomToJoin = action;
                    _joinRoomResponse = _tcpClient.joinRoom(_roomToJoin);
                }

                if (_joinRoomInitiated) {
                    if (!_joinRoomResponse.has_value()) {
                        _joinRoomResponse = _tcpClient.joinRoom(_roomToJoin);
                    }
                    if (_joinRoomResponse.has_value()) {
                        if (_joinRoomResponse.value()) {
                            _currentState = ClientState::LOBBY;
                        }
                        _joinRoomInitiated = false;
                        _joinRoomResponse = std::nullopt;
                        _roomToJoin = -1;
                    }
                }
                break;
            }
            case ClientState::LOBBY: {
                if (GetTime() - _lastLobbyUpdate > 0.5) {
                    _lobbyState = _tcpClient.getLobbyState();
                    _lastLobbyUpdate = GetTime();
                }

                if (_tcpClient.getLobbyState().gameIsStarting) {
                    _currentState = ClientState::IN_GAME;
                    break;
                }

                if (_renderer->drawLobby(_lobbyState, _connectRes.playerId)) {
                    _tcpClient.sendStartGameRequest();
                }
                break;
            }
            case ClientState::IN_GAME: {
                if (!_gameInstance) {
                    _gameInstance = std::make_unique<RTypeClient>(_serverIp, _tcpClient, _connectRes, _config.keybinds);
                    std::cout << "[Game] Starting game tick loop..." << std::endl;
                }

                _gameInstance->tick();

                if (_gameInstance->getStatus() == InGameStatus::QUITTING) {
                    _gameInstance.reset();
                    _currentState = ClientState::EXITING;
                } else if (_gameInstance->getStatus() == InGameStatus::KICKED) {
                    _gameInstance.reset();
                    _currentState = ClientState::KICKED;
                }
                break;
            }
            case ClientState::KICKED: {
                _renderer->drawKickedScreen();
                if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    _currentState = ClientState::EXITING;
                }
                break;
            }
            case ClientState::EXITING:
                break;
        }

        EndDrawing();
    }
}