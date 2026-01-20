/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPServer
*/

#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_

#include <cstdint>
#include <iostream>
#include <thread>
#include <cstring>
#include <array>
#include <unordered_map>
#include <map>
#include <atomic>
#include <memory>

#include "Client/Asio.hpp"
#include "Network/RingBuffer.hpp"
#include "Network/Packet.hpp"
#include "Network/INetworkHandler.hpp"
#include "Network/Protocole/ProtocoleUDP.hpp"
#include "Clock.hpp"
#include "Client/Windows.hpp"

/**
 * @file UDPServer.hpp
 * @brief Handles UDP communication for real-time game updates.
 */

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
     * @param rooms Reference to the shared map of rooms.
     * @param clock Reference to the shared Clock.
     */
    // UDPServer(int port, std::map<int, std::shared_ptr<Game>>& rooms, Clock& clock);

    UDPServer(int port, Network::INetworkHandler* handler, Clock& clock);

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
        Network::Packet pkt;
        pkt.addr = addr;
        pkt.length = sizeof(T);

        static_assert(sizeof(T) <= pkt.data.size(), "Packet too large!");

        std::memcpy(pkt.data.data(), &msg, sizeof(T));
        _outgoing.push(pkt);
    }

    /**
     * @brief Queues a raw data message to be sent to a specific client.
     * @param data Pointer to the raw data buffer.
     * @param length The length of the data to send.
     * @param clientAddr The destination address.
     */
    void queueMessage(const char* data, size_t length, const sockaddr_in& clientAddr);

private:
    asio::io_context _io_context; /**< ASIO IO context */
    asio::ip::udp::socket _socket; /**< UDP socket */

    std::atomic<bool> _running; /**< Running state flag */
    // std::map<int, std::shared_ptr<Game>>& _rooms; /**< Reference to shared rooms */

    Network::INetworkHandler* _handler;
    const Clock& _clock; /**< Reference to the server clock */

    std::thread _recvThread; /**< Thread for receiving packets */
    std::thread _sendThread; /**< Thread for sending packets */
    std::thread _processThread; /**< Thread for processing logic */

    Network::RingBuffer<Network::Packet, 1024> _incoming; /**< Buffer for incoming packets */
    Network::RingBuffer<Network::Packet, 1024> _outgoing; /**< Buffer for outgoing packets */

    std::unordered_map<uint32_t, ClientInfo> _clients; /**< Map of connected clients */

    /**
     * @brief The main loop for receiving incoming UDP packets.
     * Listens on the socket and pushes received packets into the incoming ring buffer.
     */
    void recvLoop();
    /**
     * @brief The main loop for sending outgoing UDP packets.
     * Pops packets from the outgoing ring buffer and sends them to their destination.
     */
    void sendLoop();
    /**
     * @brief The main loop for processing received packets.
     * Pops packets from the incoming ring buffer and passes them to handlePacket.
     */
    void processLoop();
    /**
     * @brief Handles a single raw UDP packet.
     * Deserializes the packet and triggers the appropriate game logic based on its type.
     * @param data Pointer to the raw packet data.
     * @param length The length of the packet data.
     * @param clientAddr The source address of the packet.
     */
    void handlePacket(const char* data, size_t length, const sockaddr_in& clientAddr);
};

#endif /* !UDPSERVER_HPP_ */
