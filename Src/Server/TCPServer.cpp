/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPServer
*/

#include "Server/TCPServer.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>

TCPServer::TCPServer(int port, std::map<int, std::shared_ptr<Game>>& rooms, std::mutex& roomsMutex, Clock& clock)
    : _io_context(),
      _acceptor(_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      _running(false),
      _rooms(rooms),
      _roomsMutex(roomsMutex),
      _clock(clock)
{
}

TCPServer::~TCPServer()
{
    stop();
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
    {
        std::lock_guard<std::mutex> lock(_serverMutex);
        _playerUsernames[playerId] = connectReq.username;
        _playerSockets[playerId] = clientSocket;
    }
    std::cout << "[TCP] Player " << playerId << " (" << connectReq.username << ") connected. Entering lobby." << std::endl;

    bool inLobby = true;
    while (inLobby && _running) {
        uint8_t msgType;
        asio::read(*clientSocket, asio::buffer(&msgType, sizeof(msgType)), ec);
        if (ec) {
            if (ec == asio::error::eof || ec == asio::error::connection_reset) {
                std::cout << "[TCP] Player " << playerId << " disconnected." << std::endl;
            } else {
                std::cerr << "[TCP] Player " << playerId << " disconnected from lobby: " << ec.message() << std::endl;
            }
            inLobby = false;
            continue;
        }

        switch (static_cast<TCPMessageType>(msgType)) {
            case TCPMessageType::LIST_ROOMS: {
                std::lock_guard<std::mutex> lock(_roomsMutex);
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
                    std::lock_guard<std::mutex> lock(_roomsMutex);
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

    {
        std::lock_guard<std::mutex> lock(_serverMutex);
        _playerUsernames.erase(playerId);
        _playerSockets.erase(playerId);
    }
    clientSocket->close();
    std::cout << "[TCP] Closed connection for player " << playerId << "." << std::endl;
}

void TCPServer::handleInRoomClient(std::shared_ptr<asio::ip::tcp::socket> clientSocket, int roomId, uint32_t playerId)
{
    std::shared_ptr<Game> game = nullptr;
    asio::error_code ec;
    bool inRoom = true;

    {
        std::lock_guard<std::mutex> lock(_roomsMutex);
        auto it = _rooms.find(roomId);
        if (it == _rooms.end()) {
            inRoom = false;
        } else {
            game = it->second;
        }
    }

    while(inRoom && _running) {
        uint8_t msgType;
        asio::read(*clientSocket, asio::buffer(&msgType, sizeof(msgType)), ec);
        if (ec) {
            if (ec == asio::error::eof || ec == asio::error::connection_reset) {
                std::cout << "[TCP] Player " << playerId << " disconnected." << std::endl;
            } else {
                std::cerr << "[TCP] Player " << playerId << " disconnected from room " << roomId << ": " << ec.message() << std::endl;
            }
            inRoom = false; continue;
        }

        switch(static_cast<TCPMessageType>(msgType)) {
            case TCPMessageType::GET_LOBBY_STATE: {
                if (game->getStatus() == GameStatus::PLAYING) {
                    GameStartingNotification notif;
                    asio::write(*clientSocket, asio::buffer(&notif, sizeof(notif)), ec);
                    if (ec) inRoom = false;
                    break;
                }

                std::lock_guard<std::mutex> lock(_roomsMutex);
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
                if (playerId == game->getHostId() && game->getStatus() == GameStatus::LOBBY) {
                    std::lock_guard<std::mutex> lock(_roomsMutex);
                    game->setStatus(GameStatus::PLAYING);
                    std::cout << "[Game] Room " << roomId << " is starting." << std::endl;
                }
                break;
            }
            case TCPMessageType::CHAT_MESSAGE: {
                uint16_t msgLen = 0;
                asio::read(*clientSocket, asio::buffer(&msgLen, sizeof(msgLen)), ec);
                if (ec) { inRoom = false; continue; }

                if (msgLen > 512) { 
                    std::cerr << "[TCP] Chat message too long from player " << playerId << ". Disconnecting." << std::endl;
                    inRoom = false; continue;
                }

                std::vector<char> buffer(msgLen);
                asio::read(*clientSocket, asio::buffer(buffer.data(), msgLen), ec);
                if (ec) { inRoom = false; continue; }

                std::string content(buffer.begin(), buffer.end());
                
                std::string fullMessage;
                {
                    std::lock_guard<std::mutex> lock(_serverMutex);
                    fullMessage = _playerUsernames[playerId] + ": " + content;
                }
                
                auto packet_to_send = std::make_shared<std::vector<uint8_t>>();
                packet_to_send->push_back(TCPMessageType::CHAT_MESSAGE);
                uint16_t fullLen = static_cast<uint16_t>(fullMessage.size());
                packet_to_send->resize(1 + sizeof(uint16_t));
                std::memcpy(packet_to_send->data() + 1, &fullLen, sizeof(fullLen));
                packet_to_send->insert(packet_to_send->end(), fullMessage.begin(), fullMessage.end());

                const auto& players = game->getPlayers();
                std::lock_guard<std::mutex> lock(_serverMutex);
                for (const auto& p : players) {
                    if (p.id == playerId)
                        continue;

                    if (_playerSockets.count(p.id)) {
                        auto destSocket = _playerSockets.at(p.id);
                        if (destSocket && destSocket->is_open()) {
                            asio::async_write(*destSocket, asio::buffer(*packet_to_send),
                                [](const asio::error_code&, std::size_t){});
                        }
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
        std::lock_guard<std::mutex> lock(_roomsMutex);
        game->removePlayerFromLobby(playerId);
    }
}

int TCPServer::createRoom()
{
    std::lock_guard<std::mutex> lock(_roomsMutex);
    int roomId = _nextRoomId++;
    _rooms[roomId] = std::make_shared<Game>();
    return roomId;
}

void TCPServer::kickPlayer(uint32_t playerId)
{
    std::shared_ptr<asio::ip::tcp::socket> socketToClose = nullptr;
    {
        std::lock_guard<std::mutex> lock(_serverMutex);
        if (_playerSockets.count(playerId)) {
            socketToClose = _playerSockets.at(playerId);
        }
    }
    if (socketToClose) {
        std::cout << "[TCP] Kicking player " << playerId << " by closing socket." << std::endl;
        socketToClose->close();
    }
}