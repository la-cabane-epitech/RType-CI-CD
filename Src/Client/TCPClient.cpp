/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPClient.cpp
*/

#include "Client/TCPClient.hpp"
#include <iostream>

TCPClient::TCPClient(const std::string& serverIp, uint16_t port)
    : _io_context(), _socket(_io_context), _serverIp(serverIp), _port(port)
{
}

TCPClient::~TCPClient()
{
    if (_socket.is_open()) {
        _socket.close();
    }
}

bool TCPClient::connectToServer()
{
    try {
        asio::ip::tcp::resolver resolver(_io_context);
        auto endpoints = resolver.resolve(_serverIp, std::to_string(_port));
        asio::connect(_socket, endpoints);
        if (!_socket_non_blocking_set) {
            _socket.non_blocking(true);
            _socket_non_blocking_set = true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << "\n";
        return false;
    }
    return true;
}

void TCPClient::disconnect()
{
    if (_socket.is_open()) {
        _socket.close();
    }
    _socket_non_blocking_set = false;
}

bool TCPClient::sendConnectRequest(const std::string& username, ConnectResponse& outResponse)
{
    try {
        _socket.non_blocking(false);

        ConnectRequest req{};
        req.type = 1;
        std::strncpy(req.username, username.c_str(), sizeof(req.username) - 1);
        req.username[sizeof(req.username) - 1] = '\0';

        asio::write(_socket, asio::buffer(&req, sizeof(req)));

        uint8_t type = 0;
        asio::read(_socket, asio::buffer(&type, 1));

        bool result = false;

        if (type == 2) {
            asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&outResponse) + 1, sizeof(outResponse) - 1));
            result = true;
        } else if (type == 3) {
            ErrorResponse err{};
            err.type = 3;

            asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&err) + 1, sizeof(err) - 1));

            std::cerr << "Server Error: " << err.message << "\n";
            result = false;
        } else {
            std::cerr << "Invalid response type from server\n";
            result = false;
        }

        _socket.non_blocking(true);
        return result;

    } catch (const std::exception& e) {
        std::cerr << "Handshake failed: " << e.what() << "\n";
        try { _socket.non_blocking(true); } catch (...) {}
        return false;
    }
}

std::vector<RoomInfo> TCPClient::getRooms()
{
    static std::vector<RoomInfo> cachedRooms;
    asio::error_code ec;

    try {
        ListRoomsRequest req;
        asio::write(_socket, asio::buffer(&req, sizeof(req)), ec);

        if (_socket.available() < (sizeof(uint8_t) + sizeof(int))) {
            return cachedRooms;
        }

        uint8_t respType = 0;
        asio::read(_socket, asio::buffer(&respType, 1), ec);

        if (respType != static_cast<uint8_t>(TCPMessageType::LIST_ROOMS_RESPONSE)) {
            return cachedRooms;
        }

        int count = 0;
        asio::read(_socket, asio::buffer(&count, sizeof(count)), ec);

        std::vector<RoomInfo> newRooms;
        for (int i = 0; i < count; ++i) {
            RoomInfo info;
            asio::read(_socket, asio::buffer(&info, sizeof(info)), ec);
            newRooms.push_back(info);
        }

        cachedRooms = newRooms;
        return newRooms;

    } catch (const std::exception& e) {
        std::cerr << "getRooms failed: " << e.what() << std::endl;
    }
    return cachedRooms;
}

std::optional<int> TCPClient::createRoom()
{
    asio::error_code ec;

    switch (_createRoomState) {
        case RequestState::IDLE: {
            std::cout << "[DEBUG] createRoom: Sending request..." << std::endl;
            CreateRoomRequest req;
            asio::write(_socket, asio::buffer(&req, sizeof(req)), ec);
            if (ec) {
                std::cerr << "createRoom send failed: " << ec.message() << std::endl;
                return std::optional<int>(-1);
            }
            _createRoomState = RequestState::SENT_REQUEST;
            return std::nullopt;
        }
        case RequestState::SENT_REQUEST: {
            if (_socket.available() < 1) {
                return std::nullopt;
            }

            _socket.non_blocking(false);

            uint8_t respType = 0;
            asio::read(_socket, asio::buffer(&respType, 1), ec);

            std::cout << "[DEBUG] createRoom: Received response type " << (int)respType << std::endl;
            if (ec) {
                std::cerr << "createRoom read type failed: " << ec.message() << std::endl;
                _createRoomState = RequestState::IDLE;
                _socket.non_blocking(true);
                return std::optional<int>(-1);
            }
            _createRoomPendingRespType = respType;

            if (_createRoomPendingRespType == TCPMessageType::CREATE_ROOM_RESPONSE) {
                int roomId = 0;
                asio::read(_socket, asio::buffer(&roomId, sizeof(roomId)), ec);
                if (ec) {
                    std::cerr << "createRoom read roomId failed: " << ec.message() << std::endl;
                    _createRoomState = RequestState::IDLE;
                    _createRoomPendingRespType = 0;
                    _socket.non_blocking(true);
                    return std::optional<int>(-1);
                }
                std::cout << "[DEBUG] createRoom: Success, RoomID=" << roomId << std::endl;
                _createRoomState = RequestState::IDLE;
                _createRoomPendingRespType = 0;
                _socket.non_blocking(true);
                return roomId;
            } else if (_createRoomPendingRespType == TCPMessageType::CONNECT_ERROR) {
                ErrorResponse err{};
                err.type = TCPMessageType::CONNECT_ERROR;
                asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&err) + 1, sizeof(err) - 1), ec);
                if (ec) {
                    std::cerr << "Error reading error response: " << ec.message() << std::endl;
                }
                std::cerr << "Server Error during createRoom: " << err.message << "\n";
                _createRoomState = RequestState::IDLE;
                _createRoomPendingRespType = 0;
                _socket.non_blocking(true);
                return std::optional<int>(-1);
            } else if (_createRoomPendingRespType == TCPMessageType::LIST_ROOMS_RESPONSE) {
                std::cout << "[DEBUG] createRoom: Draining stray LIST_ROOMS_RESPONSE" << std::endl;
                int count = 0;
                asio::read(_socket, asio::buffer(&count, sizeof(count)), ec);
                if (!ec && count > 0) {
                    std::vector<char> trash(count * sizeof(RoomInfo));
                    asio::read(_socket, asio::buffer(trash), ec);
                }
                _socket.non_blocking(true);

                return std::nullopt;
            } else {
                std::cerr << "Invalid response type from server for createRoom: " << static_cast<int>(_createRoomPendingRespType) << "\n";
                _createRoomState = RequestState::IDLE;
                _createRoomPendingRespType = 0;
                _socket.non_blocking(true);
                return std::optional<int>(-1);
            }
        }
        default:
            _createRoomState = RequestState::IDLE;
            return std::optional<int>(-1);
    }
}

std::optional<bool> TCPClient::joinRoom(int roomId)
{
    asio::error_code ec;

    switch (_joinRoomState) {
        case RequestState::IDLE: {
            std::cout << "[DEBUG] joinRoom: Sending request for RoomID=" << roomId << "..." << std::endl;
            JoinRoomRequest req;
            req.roomId = roomId;
            asio::write(_socket, asio::buffer(&req, sizeof(req)), ec);
            if (ec) {
                std::cerr << "joinRoom send failed: " << ec.message() << std::endl;
                return std::optional<bool>(false);
            }
            _joinRoomState = RequestState::SENT_REQUEST;
            return std::nullopt;
        }
        case RequestState::SENT_REQUEST: {
            if (_socket.available() < 1) {
                return std::nullopt;
            }

            _socket.non_blocking(false);

            uint8_t respType = 0;
            asio::read(_socket, asio::buffer(&respType, 1), ec);

            std::cout << "[DEBUG] joinRoom: Received response type " << (int)respType << std::endl;
            if (ec) {
                std::cerr << "joinRoom read type failed: " << ec.message() << std::endl;
                _joinRoomState = RequestState::IDLE;
                _socket.non_blocking(true);
                return std::optional<bool>(false);
            }
            _joinRoomPendingRespType = respType;

            if (_joinRoomPendingRespType == TCPMessageType::JOIN_ROOM_RESPONSE) {
                int status = 0;
                asio::read(_socket, asio::buffer(&status, sizeof(status)), ec);
                if (ec) {
                    std::cerr << "joinRoom read status failed: " << ec.message() << std::endl;
                    _joinRoomState = RequestState::IDLE;
                    _joinRoomPendingRespType = 0;
                    _socket.non_blocking(true);
                    return std::optional<bool>(false);
                }
                std::cout << "[DEBUG] joinRoom: Success, Status=" << status << std::endl;
                _joinRoomState = RequestState::IDLE;
                _joinRoomPendingRespType = 0;
                _socket.non_blocking(true);
                return status == 1;
            } else if (_joinRoomPendingRespType == TCPMessageType::CONNECT_ERROR) {
                ErrorResponse err{};
                err.type = TCPMessageType::CONNECT_ERROR;
                asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&err) + 1, sizeof(err) - 1), ec);
                if (ec) {
                    std::cerr << "Error reading error response: " << ec.message() << std::endl;
                }
                std::cerr << "Server Error during joinRoom: " << err.message << "\n";
                _joinRoomState = RequestState::IDLE;
                _joinRoomPendingRespType = 0;
                _socket.non_blocking(true);
                return std::optional<bool>(false);
            } else if (_joinRoomPendingRespType == TCPMessageType::LIST_ROOMS_RESPONSE) {
                std::cout << "[DEBUG] joinRoom: Draining stray LIST_ROOMS_RESPONSE" << std::endl;
                int count = 0;
                asio::read(_socket, asio::buffer(&count, sizeof(count)), ec);
                if (!ec && count > 0) {
                    std::vector<char> trash(count * sizeof(RoomInfo));
                    asio::read(_socket, asio::buffer(trash), ec);
                }
                _socket.non_blocking(true);

                return std::nullopt;
            } else {
                std::cerr << "Invalid response type from server for joinRoom: " << static_cast<int>(_joinRoomPendingRespType) << "\n";
                _joinRoomState = RequestState::IDLE;
                _joinRoomPendingRespType = 0;
                _socket.non_blocking(true);
                return std::optional<bool>(false);
            }
        }
        default:
            _joinRoomState = RequestState::IDLE;
            return std::optional<bool>(false);
    }
}

void TCPClient::sendStartGameRequest()
{
    std::cout << "[DEBUG] Sending START_GAME_REQUEST to server..." << std::endl;
    asio::error_code ec;
    StartGameRequest req;
    asio::write(_socket, asio::buffer(&req, sizeof(req)), ec);
    if (ec) {
        std::cerr << "sendStartGameRequest failed: " << ec.message() << std::endl;
    } else {
        std::cout << "[DEBUG] START_GAME_REQUEST sent successfully." << std::endl;
    }
}

LobbyState TCPClient::getLobbyState()
{
    static LobbyState cachedState;
    asio::error_code ec;

    try {
        GetLobbyStateRequest req;
        asio::write(_socket, asio::buffer(&req, sizeof(req)), ec);
        if (ec) return cachedState;

        int max_loops = 10;
        while (_socket.available() > 0 && max_loops > 0) {
            max_loops--;

            _socket.non_blocking(false);

            uint8_t respType = 0;
            asio::read(_socket, asio::buffer(&respType, 1), ec);

            if (ec) break;

            std::cout << "[DEBUG] getLobbyState: Processing packet type " << (int)respType << std::endl;
            if (respType == TCPMessageType::LOBBY_STATE_RESPONSE) {
                uint32_t hostId = 0;
                int32_t playerCount = 0;

                std::vector<asio::mutable_buffer> bufs;
                bufs.push_back(asio::buffer(&hostId, sizeof(hostId)));
                bufs.push_back(asio::buffer(&playerCount, sizeof(playerCount)));
                asio::read(_socket, bufs, ec);

                if (!ec) {
                    std::cout << "[DEBUG] getLobbyState: HostID=" << hostId << ", PlayerCount=" << playerCount << std::endl;
                    std::vector<LobbyPlayerInfo> players;
                    for (int i = 0; i < playerCount; ++i) {
                        LobbyPlayerInfo pInfo;
                        asio::read(_socket, asio::buffer(&pInfo, sizeof(pInfo)), ec);
                        if (ec) break;
                        players.push_back(pInfo);
                        std::cout << "[DEBUG]   - Player[" << i << "]: ID=" << pInfo.playerId << ", Name=" << pInfo.username << std::endl;
                    }
                    if (!ec) {
                        cachedState.hostId = hostId;
                        cachedState.players = players;
                        cachedState.gameIsStarting = false;
                    }
                }
            } else if (respType == TCPMessageType::GAME_STARTING_NOTIFICATION) {
                cachedState.gameIsStarting = true;
                std::cout << "[DEBUG] getLobbyState: Game Starting Notification received" << std::endl;
            } else if (respType == TCPMessageType::LIST_ROOMS_RESPONSE) {
                std::cout << "[DEBUG] getLobbyState: Draining stray LIST_ROOMS_RESPONSE" << std::endl;
                int count = 0;
                asio::read(_socket, asio::buffer(&count, sizeof(count)), ec);
                if (!ec && count > 0) {
                    std::vector<char> trash(count * sizeof(RoomInfo));
                    asio::read(_socket, asio::buffer(trash), ec);
                }
            } else if (respType == TCPMessageType::CREATE_ROOM_RESPONSE) {
                std::cout << "[DEBUG] getLobbyState: Draining stray CREATE_ROOM_RESPONSE" << std::endl;
                int32_t rid;
                asio::read(_socket, asio::buffer(&rid, sizeof(rid)), ec);
            } else if (respType == TCPMessageType::JOIN_ROOM_RESPONSE) {
                std::cout << "[DEBUG] getLobbyState: Draining stray JOIN_ROOM_RESPONSE" << std::endl;
                int32_t status;
                asio::read(_socket, asio::buffer(&status, sizeof(status)), ec);
            } else if (respType == TCPMessageType::CONNECT_ERROR) {
                ErrorResponse err{};
                asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&err) + 1, sizeof(err) - 1), ec);
            } else {
                std::cerr << "Unknown packet type in getLobbyState: " << (int)respType << std::endl;
            }

            _socket.non_blocking(true);
        }

    } catch (const std::exception& e) {
        std::cerr << "getLobbyState failed: " << e.what() << std::endl;
        try { _socket.non_blocking(true); } catch(...) {}
    }
    return cachedState;
}

bool TCPClient::checkConnection()
{
    if (!_socket.is_open()) {
        return false;
    }

    asio::error_code ec;
    _socket.non_blocking(true, ec);
    if (ec) { return false; }

    char d;
    _socket.read_some(asio::buffer(&d, 0), ec);

    asio::error_code ec_block;
    _socket.non_blocking(false, ec_block);
    if (ec_block) { return false; }

    if (ec == asio::error::would_block) {
        return true;
    }

    if (ec) {
        _socket.close();
        return false;
    }

    return true;
}

void TCPClient::sendChatMessage(const std::string& message)
{
    try {
        uint8_t type = TCPMessageType::CHAT_MESSAGE;
        uint16_t length = static_cast<uint16_t>(message.size());

        std::vector<uint8_t> packet;
        packet.push_back(type);
        packet.resize(3);
        std::memcpy(&packet[1], &length, sizeof(length));
        packet.insert(packet.end(), message.begin(), message.end());

        asio::write(_socket, asio::buffer(packet));
    } catch (const std::exception& e) {
        std::cerr << "sendChatMessage failed: " << e.what() << std::endl;
    }
}

std::vector<std::string> TCPClient::receiveChatMessages()
{
    std::vector<std::string> messages;
    try {
        while (_socket.available() >= 3) {
            std::array<uint8_t, 3> header;
            _socket.receive(asio::buffer(header), asio::socket_base::message_peek);

            uint8_t type = header[0];
            uint16_t length = 0;
            std::memcpy(&length, &header[1], sizeof(length));

            if (type == TCPMessageType::CHAT_MESSAGE) {
                if (_socket.available() >= 3 + length) {
                    std::vector<char> buffer(3 + length);
                    asio::read(_socket, asio::buffer(buffer));
                    messages.emplace_back(buffer.data() + 3, length);
                } else {
                    break;
                }
            } else {

                uint8_t trash;
                asio::read(_socket, asio::buffer(&trash, 1));
            }
        }
    } catch (...) {
    }
    return messages;
}
