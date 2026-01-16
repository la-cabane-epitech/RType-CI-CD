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
    } catch (const std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool TCPClient::sendConnectRequest(const std::string& username, ConnectResponse& outResponse)
{
    try {
        ConnectRequest req{};
        req.type = 1;
        std::strncpy(req.username, username.c_str(), sizeof(req.username) - 1);
        req.username[sizeof(req.username) - 1] = '\0';

        asio::write(_socket, asio::buffer(&req, sizeof(req)));

        uint8_t type = 0;
        asio::read(_socket, asio::buffer(&type, 1));

        if (type == 2) {
            outResponse.type = 2;
            asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&outResponse) + 1, sizeof(outResponse) - 1));
            return true;
        }

        if (type == 3) {
            ErrorResponse err{};
            err.type = 3;

            asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&err) + 1, sizeof(err) - 1));

            std::cerr << "Server Error: " << err.message << "\n";
            return false;
        }

        std::cerr << "Invalid response type from server\n";
        return false;

    } catch (const std::exception& e) {
        std::cerr << "Handshake failed: " << e.what() << "\n";
        return false;
    }
}

std::vector<RoomInfo> TCPClient::getRooms()
{
    std::vector<RoomInfo> rooms;
    try {
        ListRoomsRequest req;
        asio::write(_socket, asio::buffer(&req, sizeof(req)));

        uint8_t respType = 0;
        asio::read(_socket, asio::buffer(&respType, 1));
        if (respType != TCPMessageType::LIST_ROOMS_RESPONSE) return rooms;

        int count = 0;
        asio::read(_socket, asio::buffer(&count, sizeof(count)));

        for (int i = 0; i < count; ++i) {
            RoomInfo info;
            asio::read(_socket, asio::buffer(&info, sizeof(info)));
            rooms.push_back(info);
        }
    } catch (const std::exception& e) {
        std::cerr << "getRooms failed: " << e.what() << std::endl;
    }
    return rooms;
}

int TCPClient::createRoom()
{
    try {
        CreateRoomRequest req;
        asio::write(_socket, asio::buffer(&req, sizeof(req)));

        uint8_t respType = 0;
        asio::read(_socket, asio::buffer(&respType, 1));
        if (respType != TCPMessageType::CREATE_ROOM_RESPONSE) return -1;

        int roomId = 0;
        asio::read(_socket, asio::buffer(&roomId, sizeof(roomId)));
        return roomId;
    } catch (const std::exception& e) {
        std::cerr << "createRoom failed: " << e.what() << std::endl;
        return -1;
    }
}

bool TCPClient::joinRoom(int roomId)
{
    try {
        JoinRoomRequest req;
        req.roomId = roomId;
        asio::write(_socket, asio::buffer(&req, sizeof(req)));

        uint8_t respType = 0;
        asio::read(_socket, asio::buffer(&respType, 1));
        if (respType != TCPMessageType::JOIN_ROOM_RESPONSE) return false;

        int status = 0;
        asio::read(_socket, asio::buffer(&status, sizeof(status)));
        return status == 1;
    } catch (const std::exception& e) {
        std::cerr << "joinRoom failed: " << e.what() << std::endl;
        return false;
    }
}

LobbyState TCPClient::getLobbyState()
{
    LobbyState state;
    try {
        GetLobbyStateRequest req;
        asio::write(_socket, asio::buffer(&req, sizeof(req)));

        uint8_t respType = 0;
        asio::read(_socket, asio::buffer(&respType, 1));

        if (respType == TCPMessageType::GAME_STARTING_NOTIFICATION) {
            state.gameIsStarting = true;
            return state;
        }

        if (respType == TCPMessageType::LOBBY_STATE_RESPONSE) {
            LobbyStateResponse resp;
            asio::read(_socket, asio::buffer(reinterpret_cast<char*>(&resp) + 1, sizeof(resp) - 1));
            state.hostId = resp.hostId;
            for (int i = 0; i < resp.playerCount; ++i) {
                LobbyPlayerInfo info;
                asio::read(_socket, asio::buffer(&info, sizeof(info)));
                state.players.push_back(info);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "getLobbyState failed: " << e.what() << std::endl;
        state.gameIsStarting = true;
    }
    return state;
}

void TCPClient::sendStartGameRequest()
{
    try {
        StartGameRequest req;
        asio::write(_socket, asio::buffer(&req, sizeof(req)));
    } catch (const std::exception& e) {
        std::cerr << "sendStartGameRequest failed: " << e.what() << std::endl;
    }
}

void TCPClient::sendChatMessage(const std::string& message)
{
    try {
        // Protocol: Type (1 byte) + Length (2 bytes) + Content
        uint8_t type = 10; // 10 = CHAT_MESSAGE
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
        // Non-blocking check: do we have enough data for a header (3 bytes)?
        while (_socket.available() >= 3) {
            // Peek header to check type and length
            std::array<uint8_t, 3> header;
            _socket.receive(asio::buffer(header), asio::socket_base::message_peek);

            uint8_t type = header[0];
            uint16_t length = 0;
            std::memcpy(&length, &header[1], sizeof(length));

            if (type == 10) { // Chat Message
                if (_socket.available() >= 3 + length) {
                    // Read the full packet
                    std::vector<char> buffer(3 + length);
                    asio::read(_socket, asio::buffer(buffer));
                    messages.emplace_back(buffer.data() + 3, length);
                } else {
                    break; // Wait for the rest of the packet
                }
            } else {
                // Unknown packet in game loop, consume 1 byte to avoid infinite loop or handle error
                // For now, we assume only chat uses TCP in-game.
                uint8_t trash;
                asio::read(_socket, asio::buffer(&trash, 1));
            }
        }
    } catch (...) {
        // Ignore errors in polling
    }
    return messages;
}
