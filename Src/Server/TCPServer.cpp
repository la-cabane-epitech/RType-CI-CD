/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPServer implementation (Network only)
*/

#include "Server/TCPServer.hpp"
#include <iostream>
#include <cstring>
#include <memory>
#include <cstdint>

TCPServer::TCPServer(int port, ITCPHandler* handler, Clock& clock)
    : _io_context(),
        _acceptor(_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
        _running(false),
        _handler(handler),
        _clock(clock)
{
}

TCPServer::~TCPServer()
{
    stop();
}

void TCPServer::start()
{
    if (_running)
        return;
    _running = true;
    _io_context.restart();
    std::cout << "[TCP] Server starting..." << std::endl;
    _acceptThread = std::thread(&TCPServer::acceptLoop, this);
}

void TCPServer::stop()
{
    bool expected = true;
    if (!_running.compare_exchange_strong(expected, false))
        return;

    std::cout << "[TCP] Server stopping..." << std::endl;

    unsigned short port = 0;
    try {
        if (_acceptor.is_open())
            port = _acceptor.local_endpoint().port();
    } catch (...) {}

    asio::error_code ec;
    _acceptor.close(ec);

    if (port != 0) {
        try {
            asio::io_context tempContext;
            asio::ip::tcp::socket dummySocket(tempContext);
            asio::ip::tcp::endpoint target(asio::ip::tcp::v4(), port);

            dummySocket.connect(target, ec);
            dummySocket.close();
        } catch (...) {
        }
    }

    std::cout << "[TCP] Server stopped. 1 (Joining accept thread)" << std::endl;
    if (_acceptThread.joinable())
        _acceptThread.join();

    std::cout << "[TCP] Server stopped. 2 (Closing client sockets)" << std::endl;

    {
        std::lock_guard<std::mutex> lock(_serverMutex);
        for (auto& pair : _playerSockets) {
            if (pair.second && pair.second->is_open()) {
                pair.second->close();
            }
        }
    }

    for (auto& thread : _clientThreads) {
        if (thread.joinable())
            thread.join();
    }
    _clientThreads.clear();
    std::cout << "[TCP] Server fully stopped." << std::endl;
}

void TCPServer::acceptLoop()
{
    while (_running) {
        auto clientSocket = std::make_shared<asio::ip::tcp::socket>(_io_context);
        asio::error_code ec;
        _acceptor.accept(*clientSocket, ec);

        if (!_running) {
            return;
        }

        if (ec) {
            if (_running)
                std::cerr << "[TCP] Accept error: " << ec.message() << std::endl;
            break;
        }
        std::cout << "[TCP] Client connection..." << std::endl;
        _clientThreads.emplace_back(&TCPServer::handleClient, this, clientSocket);
    }
}

void TCPServer::handleClient(std::shared_ptr<asio::ip::tcp::socket> clientSocket)
{
    asio::error_code ec;

    ConnectRequest connectReq{};
    asio::read(*clientSocket, asio::buffer(&connectReq, sizeof(connectReq)), ec);
    if (ec || connectReq.type != TCPMessageType::CONNECT) {
        clientSocket->close();
        std::cout << "[TCP] Client disconnected. (Invalid connect request)" << std::endl;
        return;
    }

    uint32_t playerId = _nextPlayerId++;
    std::string username = connectReq.username;

    {
        std::lock_guard<std::mutex> lock(_serverMutex);
        _playerSockets[playerId] = clientSocket;
        _playerUsernames[playerId] = username;
    }

    ConnectResponse connectRes;
    connectRes.type = TCPMessageType::CONNECT_OK;
    connectRes.playerId = playerId;
    connectRes.udpPort = 5252;
        connectRes.serverTimeMs = _clock.getElapsedTimeMs();

    asio::write(*clientSocket, asio::buffer(&connectRes, sizeof(connectRes)), ec);

    if (ec)
        return;

    bool inLobby = true;
    while (inLobby && _running) {
        uint8_t msgType;
        asio::read(*clientSocket, asio::buffer(&msgType, sizeof(msgType)), ec);
        if (ec) {
            inLobby = false; continue;
        }

        switch (static_cast<TCPMessageType>(msgType)) {
            case TCPMessageType::LIST_ROOMS: {
                auto rooms = _handler->onGetRooms();
                ListRoomsResponse resp;
                resp.count = static_cast<int>(rooms.size());
                asio::write(*clientSocket, asio::buffer(&resp, sizeof(resp)), ec);
                for (auto const& r : rooms) {
                    RoomInfo info;

                    info.id = r.id;
                    info.playerCount = r.playerCount;
                    info.maxPlayers = 4;
                    asio::write(*clientSocket, asio::buffer(&info, sizeof(info)), ec);
                }
                break;
            }
            case TCPMessageType::CREATE_ROOM: {
                int newRoomId = _handler->onCreateRoom();
                CreateRoomResponse resp{.roomId = newRoomId};
                asio::write(*clientSocket, asio::buffer(&resp, sizeof(resp)), ec);
                break;
            }
            case TCPMessageType::JOIN_ROOM: {
                JoinRoomRequest req;
                asio::read(*clientSocket, asio::buffer(&req.roomId, sizeof(req.roomId)), ec);

                bool success = _handler->onJoinRoom(req.roomId, playerId, username);
                JoinRoomResponse resp;
                resp.status = success ? 1 : 0;
                asio::write(*clientSocket, asio::buffer(&resp, sizeof(resp)), ec);

                if (success) {
                    {
                        std::lock_guard<std::mutex> lock(_serverMutex);
                        _playerRoomMap[playerId] = req.roomId;
                    }
                    handleInRoomClient(clientSocket, req.roomId, playerId);
                    {
                        std::lock_guard<std::mutex> lock(_serverMutex);
                        _playerRoomMap.erase(playerId);
                    }
                    inLobby = false;
                }
                break;
            }
            default:
                break;
        }
    }
    {
        std::lock_guard<std::mutex> lock(_serverMutex);
        _playerSockets.erase(playerId);
        _playerUsernames.erase(playerId);
    }
    clientSocket->close();
}

void TCPServer::handleInRoomClient(std::shared_ptr<asio::ip::tcp::socket> clientSocket, int roomId, uint32_t playerId)
{
    asio::error_code ec;
    bool inRoom = true;

    while(inRoom && _running) {
        uint8_t msgType;
        asio::read(*clientSocket, asio::buffer(&msgType, sizeof(msgType)), ec);
        if (ec) {
            _handler->onPlayerDisconnect(playerId, roomId);
            break;
        }

        if (static_cast<TCPMessageType>(msgType) == TCPMessageType::START_GAME_REQUEST) {
            _handler->onStartGame(roomId, playerId);
        }
        else if (static_cast<TCPMessageType>(msgType) == TCPMessageType::GET_LOBBY_STATE) {
            if (_handler->isGameStarting(roomId)) {
                GameStartingNotification notif;
                asio::write(*clientSocket, asio::buffer(&notif, sizeof(notif)), ec);
            } else {
                uint32_t hostId = 0;
                std::vector<std::pair<uint32_t, std::string>> players;
                _handler->onGetLobbyState(roomId, hostId, players);

                LobbyStateResponse resp;
                resp.type = TCPMessageType::LOBBY_STATE_RESPONSE;
                resp.hostId = hostId;
                resp.playerCount = static_cast<int32_t>(players.size());
                asio::write(*clientSocket, asio::buffer(&resp, sizeof(resp)), ec);

                for (const auto& player : players) {
                    LobbyPlayerInfo info;
                    info.playerId = player.first;
                    std::strncpy(info.username, player.second.c_str(), 31);
                    info.username[31] = '\0';
                    asio::write(*clientSocket, asio::buffer(&info, sizeof(info)), ec);
                }
            }
        } else if (static_cast<TCPMessageType>(msgType) == TCPMessageType::CHAT_MESSAGE) {
            uint16_t length = 0;
            asio::read(*clientSocket, asio::buffer(&length, sizeof(length)), ec);
            if (!ec && length > 0) {
                std::vector<char> msgBuffer(length);
                asio::read(*clientSocket, asio::buffer(msgBuffer), ec);
                if (!ec) {
                    std::string msg(msgBuffer.begin(), msgBuffer.end());
                    std::string senderName = "Unknown";
                    {
                        std::lock_guard<std::mutex> lock(_serverMutex);
                        if (_playerUsernames.count(playerId)) senderName = _playerUsernames[playerId];
                    }
                    std::string fullMsg = senderName + ": " + msg;

                    std::vector<uint8_t> packet;
                    packet.push_back(static_cast<uint8_t>(TCPMessageType::CHAT_MESSAGE));
                    uint16_t newLen = static_cast<uint16_t>(fullMsg.size());
                    packet.resize(3);
                    std::memcpy(&packet[1], &newLen, sizeof(newLen));
                    packet.insert(packet.end(), fullMsg.begin(), fullMsg.end());

                    std::lock_guard<std::mutex> lock(_serverMutex);
                    for (auto const& [pId, sock] : _playerSockets) {
                        if (pId != playerId && _playerRoomMap.count(pId) && _playerRoomMap[pId] == roomId) {
                            asio::write(*sock, asio::buffer(packet), ec);
                        }
                    }
                }
            }
        }
    }
}

void TCPServer::kickPlayer(uint32_t playerId)
{
    std::lock_guard<std::mutex> lock(_serverMutex);
    auto it = _playerSockets.find(playerId);
    if (it != _playerSockets.end()) {
        if (it->second && it->second->is_open()) {
            try {
                it->second->close();
            } catch (...) {}
        }
    }
}