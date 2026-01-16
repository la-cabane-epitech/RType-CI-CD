/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPServer
*/

#include "Server/TCPServer.hpp"
#include "Server/Utils.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace NetworkUtils;

// Map globale pour stocker les sockets des clients (PlayerID -> Socket)
static std::map<uint32_t, int> g_clientSockets;
static std::mutex g_socketMapMutex;

TCPServer::TCPServer(int port, std::map<int, std::shared_ptr<Game>>& rooms, Clock& clock)
    : _rooms(rooms), _clock(clock)
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_sockfd, (sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Failed to bind TCP socket");
    if (listen(_sockfd, 10) < 0)
    throw std::runtime_error("Failed to listen on TCP socket");
    _running = false;
}

TCPServer::~TCPServer()
{
    stop();
}

void TCPServer::stop()
{
    if (!_running)
        return;
    _running = false;

    std::cout << "[TCP] Server stopping..." << std::endl;
    close(_sockfd);

    if (_acceptThread.joinable())
        _acceptThread.join();

    for (auto& thread : _clientThread) {
        if (thread.joinable())
            thread.join();
    }
}

void TCPServer::start()
{
    if (_running)
        return;
    _running = true;

    std::cout << "[TCP] Server starting..." << std::endl;
    _acceptThread = std::thread(&TCPServer::acceptLoop, this);
}

void TCPServer::acceptLoop()
{
    while (_running) {
        int clientSock = accept(_sockfd, nullptr, nullptr);
        if (clientSock < 0)
            continue;
        std::cout << "[TCP] Client connection..." << std::endl;
        _clientThread.emplace_back(&TCPServer::handleClient, this, clientSock);
    }
}


void TCPServer::handleClient(int clientSock)
{
    ConnectRequest connectReq {};
    if (!recvAll(clientSock, &connectReq, sizeof(connectReq))) {
        std::cerr << "[TCP] Failed to receive ConnectRequest" << std::endl;
        close(clientSock);
        return;
    }
    std::cout << "[TCP] Username = " << connectReq.username << std::endl;

    if (connectReq.type != TCPMessageType::CONNECT) {
        ErrorResponse err {};
        err.type = TCPMessageType::CONNECT_ERROR;
        strncpy(err.message, "Invalid request type", sizeof(err.message) - 1);
        err.message[sizeof(err.message)-1] = '\0';
        sendAll(clientSock, &err, sizeof(err));
        close(clientSock);
        return;
    }

    ConnectResponse connectRes {};
    connectRes.type = TCPMessageType::CONNECT_OK;
    connectRes.playerId = _nextPlayerId++;
    connectRes.udpPort = 5252;
    connectRes.clock = _clock;

    if (!sendAll(clientSock, &connectRes, sizeof(connectRes))) {
        std::cerr << "[TCP] Failed to send ConnectResponse" << std::endl;
        close(clientSock);
        return;
    }
    uint32_t playerId = connectRes.playerId;
    _playerUsernames[playerId] = connectReq.username;
    std::cout << "[TCP] Player " << playerId << " (" << connectReq.username << ") connected. Entering lobby." << std::endl;

    {
        std::lock_guard<std::mutex> lock(g_socketMapMutex);
        g_clientSockets[playerId] = clientSock;
    }

    bool inLobby = true;
    while (inLobby && _running) {
        uint8_t msgType;
        if (!recvAll(clientSock, &msgType, sizeof(msgType))) {
            inLobby = false;
            continue;
        }

        switch (static_cast<TCPMessageType>(msgType)) {
            case TCPMessageType::LIST_ROOMS: {
                std::lock_guard<std::mutex> lock(_serverMutex);
                ListRoomsResponse resp;
                resp.count = _rooms.size();
                sendAll(clientSock, &resp, sizeof(resp));
                for (auto const& [id, game] : _rooms) {
                    RoomInfo info;
                    info.id = id;
                    info.playerCount = game->getPlayerCount();
                    info.maxPlayers = 4;
                    sendAll(clientSock, &info, sizeof(info));
                }
                break;
            }
            case TCPMessageType::CREATE_ROOM: {
                std::lock_guard<std::mutex> lock(_serverMutex);
                int newRoomId = createRoom();
                CreateRoomResponse resp;
                resp.roomId = newRoomId;
                sendAll(clientSock, &resp, sizeof(resp));
                break;
            }
            case TCPMessageType::JOIN_ROOM: 
            {
                JoinRoomRequest req;
                if (!recvAll(clientSock, &req.roomId, sizeof(req.roomId))) {
                    inLobby = false; continue;
                }
                JoinRoomResponse resp;
                {
                    std::lock_guard<std::mutex> lock(_serverMutex);
                    auto it = _rooms.find(req.roomId);
                    if (it != _rooms.end() && it->second->getStatus() == GameStatus::LOBBY) {
                        it->second->addPlayer(playerId, _playerUsernames[playerId].c_str());
                        resp.status = 1;
                        inLobby = false;
                    } else {
                        resp.status = 0;
                    }
                }
                sendAll(clientSock, &resp, sizeof(resp));

                if (resp.status == 1) {
                    handleInRoomClient(clientSock, req.roomId, playerId);
                }
                break;
            }
            default:
                std::cerr << "[TCP] Received unknown message type: " << (int)msgType << std::endl;
                inLobby = false;
                break;
        }
    }

    close(clientSock);
    {
        std::lock_guard<std::mutex> lock(g_socketMapMutex);
        g_clientSockets.erase(playerId);
    }
    _playerUsernames.erase(playerId);
    std::cout << "[TCP] Closed connection for player " << playerId << "." << std::endl;
}

void TCPServer::handleInRoomClient(int clientSock, int roomId, uint32_t playerId)
{
    std::shared_ptr<Game> game = nullptr;
    {
        std::lock_guard<std::mutex> lock(_serverMutex);
        game = _rooms.at(roomId);
    }

    bool inRoom = true;
    while(inRoom && _running) {
        uint8_t msgType;
        if (!recvAll(clientSock, &msgType, sizeof(msgType))) {
            inRoom = false; continue;
        }

        switch(static_cast<TCPMessageType>(msgType)) {
            case TCPMessageType::GET_LOBBY_STATE: {
                if (game->getStatus() == GameStatus::PLAYING) {
                    GameStartingNotification notif;
                    sendAll(clientSock, &notif, sizeof(notif));
                    break;
                }
                LobbyStateResponse resp;
                resp.hostId = game->getHostId();
                const auto& players = game->getPlayers();
                resp.playerCount = players.size();
                sendAll(clientSock, &resp, sizeof(resp));
                for(const auto& player : players) {
                    LobbyPlayerInfo info;
                    info.playerId = player.id;
                    strncpy(info.username, player.username, sizeof(info.username) - 1);
                    sendAll(clientSock, &info, sizeof(info));
                }
                break;
            }
            case TCPMessageType::START_GAME_REQUEST: {
                if (playerId == game->getHostId()) {
                    game->setStatus(GameStatus::PLAYING);
                    std::cout << "[Game] Room " << roomId << " is starting." << std::endl;
                }
                break;
            }
            case TCPMessageType::CHAT_MESSAGE: {
                uint16_t msgLen = 0;
                if (!recvAll(clientSock, &msgLen, sizeof(msgLen))) {
                    inRoom = false; continue;
                }
                std::vector<char> buffer(msgLen);
                if (!recvAll(clientSock, buffer.data(), msgLen)) {
                    inRoom = false; continue;
                }
                std::string content(buffer.begin(), buffer.end());
                
                // Formatage du message : "Username: Message"
                std::string fullMessage = _playerUsernames[playerId] + ": " + content;
                
                // Préparation du paquet à envoyer
                std::vector<uint8_t> packet;
                packet.push_back(TCPMessageType::CHAT_MESSAGE);
                uint16_t fullLen = static_cast<uint16_t>(fullMessage.size());
                packet.resize(3);
                std::memcpy(&packet[1], &fullLen, sizeof(fullLen));
                packet.insert(packet.end(), fullMessage.begin(), fullMessage.end());

                // Diffusion aux autres joueurs de la room
                const auto& players = game->getPlayers();
                std::lock_guard<std::mutex> lock(g_socketMapMutex);
                for (const auto& p : players) {
                    if (p.id != playerId && g_clientSockets.count(p.id)) {
                        // On envoie aux autres (l'expéditeur l'affiche déjà localement)
                        sendAll(g_clientSockets[p.id], packet.data(), packet.size());
                    }
                }
                break;
            }
            default:
                inRoom = false;
                break;
        }
    }
    if (game->getStatus() != GameStatus::PLAYING) {
        game->removePlayerFromLobby(playerId);
    }
}

int TCPServer::createRoom()
{
    int roomId = _nextRoomId++;
    _rooms[roomId] = std::make_shared<Game>();
    return roomId;
}