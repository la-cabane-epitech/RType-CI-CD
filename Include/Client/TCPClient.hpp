#ifndef TCPCLIENT_HPP_
#define TCPCLIENT_HPP_

#include <string>
#include <stdint.h>
#include <cstring>
#include <vector>
#include "Protocole/ProtocoleTCP.hpp"
#include "Client/Asio.hpp"

/**
 * @struct LobbyState
 * @brief Represents the state of a game lobby.
 *
 * Contains information about whether the game is starting, the host's ID,
 * and a list of players currently in the lobby.
 */
struct LobbyState {
    bool gameIsStarting = false; /**< Flag indicating if the game is about to start. */
    uint32_t hostId = 0; /**< The ID of the player who is the host of the lobby. */
    std::vector<LobbyPlayerInfo> players; /**< A list of players in the lobby. */
};

/**
 * @class TCPClient
 * @brief Handles the TCP connection for the initial handshake with the server.
 */
class TCPClient {
private:
    asio::io_context _io_context; /**< ASIO IO context */
    asio::ip::tcp::socket _socket; /**< TCP socket */
    std::string _serverIp; /**< Server IP address */
    uint16_t _port; /**< Server TCP port */

public:
    /**
     * @brief Construct a new TCPClient object.
     * @param serverIp IP address of the server.
     * @param port TCP port of the server.
     */
    TCPClient(const std::string& serverIp, uint16_t port);

    /**
     * @brief Destroy the TCPClient object.
     */
    ~TCPClient();

    /**
     * @brief Establishes a connection to the server.
     * @return true if connection is successful, false otherwise.
     */
    bool connectToServer();

    /**
     * @brief Sends a connection request (handshake) to the server.
     * @param username The username of the player.
     * @param outResponse Reference to store the server's response.
     * @return true if the request was sent and response received successfully, false otherwise.
     */
    bool sendConnectRequest(const std::string& username, ConnectResponse& outResponse);

    /**
     * @brief Requests the list of available rooms from the server.
     * @return A vector containing information about available rooms.
     */
    std::vector<RoomInfo> getRooms();

    /**
     * @brief Requests the server to create a new room.
     * @return The ID of the created room, or -1 on failure.
     */
    int createRoom();

    /**
     * @brief Requests to join a specific room.
     * @param roomId The ID of the room to join.
     * @return true if joined successfully, false otherwise.
     */
    bool joinRoom(int roomId);

    /**
     * @brief Polls the server for the current state of the lobby.
     * @return A LobbyState struct.
     */
    LobbyState getLobbyState();

    /**
     * @brief Sends a request to the server to start the game.
     * This can typically only be done by the host of the lobby.
     */
    void sendStartGameRequest();

    /**
     * @brief Sends a chat message to the server.
     * @param message The content of the message.
     */
    void sendChatMessage(const std::string& message);

    /**
     * @brief Checks for incoming chat messages without blocking.
     * @return A vector of received messages.
     */
    std::vector<std::string> receiveChatMessages();

};

#endif // TCPCLIENT_HPP_
