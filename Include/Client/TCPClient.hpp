#ifndef TCPCLIENT_HPP_
#define TCPCLIENT_HPP_

#include <string>
#include <stdint.h>
#include <cstring>
#include "Protocole/ProtocoleTCP.hpp"
#include "Client/Asio.hpp"

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
};

#endif // TCPCLIENT_HPP_
