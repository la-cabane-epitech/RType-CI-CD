/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/
#include "Client/Ray.hpp"
#include "Client/TCPClient.hpp"
#include "Client/RTypeClient.hpp"
#include "Client/ConfigManager.hpp"
#include <iostream>

enum class ClientState {
    USERNAME_INPUT,
    MAIN_MENU,
    OPTIONS,
    ROOM_SELECTION,
    LOBBY,
    IN_GAME,
    EXITING
};

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " <server_ip>\n";
        return 1;
    }
    std::string serverIp = av[1];

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "R-Type Client");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    const std::string CONFIG_FILE = "config_file";
    Config config = ConfigManager::loadConfig(CONFIG_FILE);

    GameState dummyState;
    Renderer renderer(dummyState);
    ClientState currentState = config.username.empty() ? ClientState::USERNAME_INPUT : ClientState::MAIN_MENU;

    TCPClient tcpClient(serverIp, 4242);
    if (!tcpClient.connectToServer()) {
        std::cerr << "Impossible de se connecter au serveur TCP\n";
        CloseWindow();
        return 1;
    }

    ConnectResponse connectRes;

    std::vector<RoomInfo> rooms;
    double lastRoomUpdate = 0;

    LobbyState lobbyState;
    double lastLobbyUpdate = 0;
    bool connected = false;

    while (currentState != ClientState::EXITING && !WindowShouldClose()) {
        BeginDrawing();
        switch (currentState) {
            case ClientState::USERNAME_INPUT: {
                if (renderer.drawUsernameInput(config.username)) {
                    ConfigManager::saveConfig(config, CONFIG_FILE);
                    currentState = ClientState::MAIN_MENU;
                }
                break;
            }
            case ClientState::MAIN_MENU: {
                MainMenuChoice choice = renderer.drawMainMenu();
                if (choice == MainMenuChoice::START) {
                    currentState = ClientState::ROOM_SELECTION;
                } else if (choice == MainMenuChoice::OPTIONS) {
                    currentState = ClientState::OPTIONS;
                }
                break;
            }

            case ClientState::OPTIONS: {
                if (renderer.drawOptionsMenu(config.keybinds)) {
                    currentState = ClientState::MAIN_MENU;
                    ConfigManager::saveConfig(config, CONFIG_FILE);
                }
                break;
            }

            case ClientState::ROOM_SELECTION: {
                if (!connected) {
                    if (!tcpClient.sendConnectRequest(config.username, connectRes)) {
                        std::cerr << "Handshake TCP échoué\n";
                        currentState = ClientState::EXITING;
                        break;
                    }
                    std::cout << "Connecté ! PlayerId: " << connectRes.playerId << ", UDP Port: " << connectRes.udpPort << "\n";
                    connected = true;
                }

                double now = GetTime();
                if (now - lastRoomUpdate > 1.0) {
                    rooms = tcpClient.getRooms();
                    lastRoomUpdate = now;
                }

                int action = renderer.drawRoomMenu(rooms);
                if (action == -2) {
                    int newRoomId = tcpClient.createRoom();
                    if (newRoomId >= 0 && tcpClient.joinRoom(newRoomId)) {
                        currentState = ClientState::LOBBY;
                    }
                } else if (action >= 0) {
                    if (tcpClient.joinRoom(action)) {
                        currentState = ClientState::LOBBY;
                    }
                }
                break;
            }

            case ClientState::LOBBY: {
                if (GetTime() - lastLobbyUpdate > 0.5) {
                    lobbyState = tcpClient.getLobbyState();
                    lastLobbyUpdate = GetTime();
                }

                if (lobbyState.gameIsStarting) {
                    currentState = ClientState::IN_GAME;
                    break;
                }

                if (renderer.drawLobby(lobbyState, connectRes.playerId)) {
                    tcpClient.sendStartGameRequest();
                }
                break;
            }

            case ClientState::IN_GAME: {
                RTypeClient client(serverIp, connectRes, config.keybinds, tcpClient);
                client.run();
                currentState = ClientState::EXITING;
                break;
            }
            case ClientState::EXITING:
                break;
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
