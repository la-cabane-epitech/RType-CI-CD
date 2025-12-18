/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPServer
*/

#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_

    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>

#include <cstdint>
#include <iostream>
#include <thread>
#include <cstring>
#include <array>
#include <unordered_map>

#include "Server/RingBuffer.hpp"
#include "Server/Packet.hpp"
#include "Protocole/ProtocoleUDP.hpp"
#include "Clock.hpp"

/**
 * @file UDPServer.hpp
 * @brief Handles UDP communication for real-time game updates.
 */

class Game;

/**
 * @struct ClientInfo
 * @brief Stores network information about a connected client.
 */
struct ClientInfo {
    sockaddr_in addr; /**< Client's UDP address */
};

/**
 * @class UDPServer
 * @brief Server class managing UDP communication.
 *
 * Handles receiving player inputs and sending game state updates.
 * Uses ring buffers for thread-safe packet processing.
 */
class UDPServer {
public:
    /**
     * @brief Construct a new UDPServer object.
     * @param port The UDP port to bind to.
     * @param game Reference to the Game instance.
     * @param clock Reference to the shared Clock.
     */
    UDPServer(int port, Game& game, Clock& clock);

    /**
     * @brief Destroy the UDPServer object.
     */
    ~UDPServer();

    /**
     * @brief Starts the receive, send, and process threads.
     */
    void start();

    /**
     * @brief Stops the server and joins threads.
     */
    void stop();

    /**
     * @brief Queues a message to be sent to a specific client.
     * @tparam T Type of the message structure.
     * @param msg The message to send.
     * @param addr The destination address.
     */
    template<typename T>
    void queueMessage(const T& msg, const sockaddr_in& addr)
    {
        Packet pkt;
        pkt.addr = addr;
        pkt.length = sizeof(T);

        static_assert(sizeof(T) <= pkt.data.size(), "Packet too large!");

        std::memcpy(pkt.data.data(), &msg, sizeof(T));
        _outgoing.push(pkt);
    }

private:
    int _sockfd; /**< UDP socket file descriptor */
    bool _running; /**< Running state flag */
    Game& _game; /**< Reference to the game logic */
    const Clock& _clock; /**< Reference to the server clock */

    std::thread _recvThread; /**< Thread for receiving packets */
    std::thread _sendThread; /**< Thread for sending packets */
    std::thread _processThread; /**< Thread for processing logic */

    RingBuffer<Packet, 1024> _incoming; /**< Buffer for incoming packets */
    RingBuffer<Packet, 1024> _outgoing; /**< Buffer for outgoing packets */

    std::unordered_map<uint32_t, ClientInfo> _clients; /**< Map of connected clients */

    void recvLoop();
    void sendLoop();
    void processLoop();
    void handlePacket(const char* data, size_t length, const sockaddr_in& clientAddr);
};

#endif /* !UDPSERVER_HPP_ */
