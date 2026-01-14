/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPServer
*/

#include "Server/TCPServer.hpp"
#include <iostream>
#include <cstring>

TCPServer::TCPServer(int port, std::map<int, std::shared_ptr<Game>>& rooms, Clock& clock)
    : _io_context(),
      _acceptor(_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      _running(false),
      _rooms(rooms),
      _clock(clock)
{
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
    _io_context.stop();
    if (_acceptor.is_open())
        _acceptor.close();

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
        auto clientSocket = std::make_shared<asio::ip::tcp::socket>(_io_context);
        asio::error_code ec;
        _acceptor.accept(*clientSocket, ec);

        if (ec) {
            if (_running)
                std::cerr << "[TCP] Accept error: " << ec.message() << std::endl;
            continue;
        }
        std::cout << "[TCP] Client connection..." << std::endl;
        _clientThread.emplace_back(&TCPServer::handleClient, this, clientSocket);
    }
}


void TCPServer::handleClient(std::shared_ptr<asio::ip::tcp::socket> clientSocket)
{
    asio::error_code ec;
    ConnectRequest connectReq{};
    asio::read(*clientSocket, asio::buffer(&connectReq, sizeof(connectReq)), ec);
    if (ec) {
        std::cerr << "[TCP] Failed to receive ConnectRequest: " << ec.message() << std::endl;
        clientSocket->close();
        return;
    }
    std::cout << "[TCP] Username = " << connectReq.username << std::endl;

    if (connectReq.type != TCPMessageType::CONNECT) {
        ErrorResponse err{};
        err.type = TCPMessageType::CONNECT_ERROR;
        strncpy(err.message, "Invalid request type", sizeof(err.message) - 1);
        err.message[sizeof(err.message)-1] = '\0';
        asio::write(*clientSocket, asio::buffer(&err, sizeof(err)), ec);
        clientSocket->close();
        return;
    }

    ConnectResponse connectRes{};
    connectRes.type = TCPMessageType::CONNECT_OK;
    connectRes.playerId = _nextPlayerId++;
    connectRes.udpPort = 5252;
    connectRes.clock = _clock;

    asio::write(*clientSocket, asio::buffer(&connectRes, sizeof(connectRes)), ec);
    if (ec) {
        std::cerr << "[TCP] Failed to send ConnectResponse: " << ec.message() << std::endl;
        clientSocket->close();
        return;
    }
    uint32_t playerId = connectRes.playerId;
    _playerUsernames[playerId] = connectReq.username;
    std::cout << "[TCP] Player " << playerId << " (" << connectReq.username << ") connected. Entering lobby." << std::endl;

    bool inLobby = true;
    while (inLobby && _running) {
        uint8_t msgType;
        asio::read(*clientSocket, asio::buffer(&msgType, sizeof(msgType)), ec);
        if (ec) {
            std::cerr << "[TCP] Player " << playerId << " disconnected from lobby: " << ec.message() << std::endl;
            inLobby = false;
            continue;
        }

        switch (static_cast<TCPMessageType>(msgType)) {
            case TCPMessageType::LIST_ROOMS: {
                std::lock_guard<std::mutex> lock(_serverMutex);
                ListRoomsResponse resp;
                resp.count = _rooms.size();
                asio::write(*clientSocket, asio::buffer(&resp, sizeof(resp)), ec);
                if (ec) { inLobby = false; break; }
                for (auto const& [id, game] : _rooms) {
                    RoomInfo info;
                    info.id = id;
                    info.playerCount = game->getPlayerCount();
                    info.maxPlayers = 4;
                    asio::write(*clientSocket, asio::buffer(&info, sizeof(info)), ec);
                    if (ec) { inLobby = false; break; }
                }
                break;
            }
            case TCPMessageType::CREATE_ROOM: {
                std::lock_guard<std::mutex> lock(_serverMutex);
                int newRoomId = createRoom();
                CreateRoomResponse resp;
                resp.roomId = newRoomId;
                asio::write(*clientSocket, asio::buffer(&resp, sizeof(resp)), ec);
                if (ec) inLobby = false;
                break;
            }
            case TCPMessageType::JOIN_ROOM: 
            {
                JoinRoomRequest req;
                asio::read(*clientSocket, asio::buffer(&req.roomId, sizeof(req.roomId)), ec);
                if (ec) { inLobby = false; continue; }

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
                asio::write(*clientSocket, asio::buffer(&resp, sizeof(resp)), ec);
                if (ec) { inLobby = false; continue; }

                if (resp.status == 1) {
                    handleInRoomClient(clientSocket, req.roomId, playerId);
                }
                break;
            }
            default:
                std::cerr << "[TCP] Received unknown message type: " << (int)msgType << std::endl;
                inLobby = false;
                break;
        }
    }

    clientSocket->close();
    _playerUsernames.erase(playerId);
    std::cout << "[TCP] Closed connection for player " << playerId << "." << std::endl;
}

void TCPServer::handleInRoomClient(std::shared_ptr<asio::ip::tcp::socket> clientSocket, int roomId, uint32_t playerId)
{
    std::shared_ptr<Game> game = nullptr;
    {
        std::lock_guard<std::mutex> lock(_serverMutex);
        game = _rooms.at(roomId);
    }

    asio::error_code ec;
    bool inRoom = true;
    while(inRoom && _running) {
        uint8_t msgType;
        asio::read(*clientSocket, asio::buffer(&msgType, sizeof(msgType)), ec);
        if (ec) {
            std::cerr << "[TCP] Player " << playerId << " disconnected from room " << roomId << ": " << ec.message() << std::endl;
            inRoom = false; continue;
        }

        if (game->getStatus() == GameStatus::PLAYING) {
            GameStartingNotification notif;
            asio::write(*clientSocket, asio::buffer(&notif, sizeof(notif)), ec);
            inRoom = false;
            continue;
        }

        switch(static_cast<TCPMessageType>(msgType)) {
            case TCPMessageType::GET_LOBBY_STATE: {
                LobbyStateResponse resp;
                resp.hostId = game->getHostId();
                const auto& players = game->getPlayers();
                resp.playerCount = players.size();
                asio::write(*clientSocket, asio::buffer(&resp, sizeof(resp)), ec);
                if (ec) { inRoom = false; break; }
                for(const auto& player : players) {
                    LobbyPlayerInfo info;
                    info.playerId = player.id;
                    strncpy(info.username, player.username, sizeof(info.username) - 1);
                    asio::write(*clientSocket, asio::buffer(&info, sizeof(info)), ec);
                    if (ec) { inRoom = false; break; }
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