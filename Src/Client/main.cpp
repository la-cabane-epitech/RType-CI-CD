/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/

#include "Client/Ray.hpp"
#include "Client/TCPClient.hpp"
#include "Client/RTypeClient.hpp"
#include <iostream>

enum class ClientState {
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
    SetTargetFPS(60);

    GameState dummyState; // For menu rendering before game state exists
    Renderer renderer(dummyState);
    ClientState currentState = ClientState::MAIN_MENU;

    TCPClient tcpClient(serverIp, 4242);
    if (!tcpClient.connectToServer()) {
        std::cerr << "Impossible de se connecter au serveur TCP\n";
        CloseWindow();
        return 1;
    }

    ConnectResponse connectRes;
    if (!tcpClient.sendConnectRequest("Player1", connectRes)) {
        std::cerr << "Handshake TCP échoué\n";
        CloseWindow();
        return 1;
    }

    std::cout << "Connecté ! PlayerId: " << connectRes.playerId
            << ", UDP Port: " << connectRes.udpPort << "\n";

    std::map<std::string, int> keybinds = {
        {"UP", KEY_W},
        {"DOWN", KEY_S},
        {"LEFT", KEY_A},
        {"RIGHT", KEY_D},
        {"SHOOT", KEY_SPACE}
    };

    std::vector<RoomInfo> rooms;
    double lastRoomUpdate = 0;

    LobbyState lobbyState;
    double lastLobbyUpdate = 0;

    while (currentState != ClientState::EXITING && !WindowShouldClose()) {
        switch (currentState) {
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
                if (renderer.drawOptionsMenu(keybinds)) {
                    currentState = ClientState::MAIN_MENU;
                    // Ici, vous pourriez sauvegarder les keybinds dans un fichier
                }
                break;
            }

            case ClientState::ROOM_SELECTION: {
                double now = GetTime();
                if (now - lastRoomUpdate > 1.0) {
                    rooms = tcpClient.getRooms();
                    lastRoomUpdate = now;
                }

                int action = renderer.drawRoomMenu(rooms);
                if (action == -2) { // Create Room
                    int newRoomId = tcpClient.createRoom();
                    if (newRoomId >= 0 && tcpClient.joinRoom(newRoomId)) {
                        currentState = ClientState::LOBBY;
                    }
                } else if (action >= 0) { // Join Room
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
                RTypeClient client(serverIp, connectRes, keybinds);
                client.run();
                currentState = ClientState::EXITING; // Ou retour au MAIN_MENU
                break;
            }
            case ClientState::EXITING:
                break;
        }
    }

    CloseWindow();
    return 0;
}
