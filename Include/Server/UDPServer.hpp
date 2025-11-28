/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPServer
*/

#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_

// ---------------- PLATFORM INCLUDES ----------------
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    using SocketType = SOCKET;
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    using SocketType = int;
#endif
// ---------------------------------------------------

#include <cstdint>
#include <iostream>
#include <thread>
#include <cstring>
#include <array>
#include <unordered_map>

#include "Server/RingBuffer.hpp"
#include "Server/Packet.hpp"
#include "Protocole/ProtocoleUDP.hpp"

class Game; // Forward declaration

struct ClientInfo {
    sockaddr_in addr;
};

class UDPServer {
public:
    UDPServer(int port, Game& game);
    ~UDPServer();

    void start();
    void stop();

    template<typename T>
    void queueMessage(const T& msg, const sockaddr_in& addr);

private:
    SocketType _sockfd;
    bool _running;
    Game& _game;

    std::thread _recvThread;
    std::thread _sendThread;
    std::thread _processThread;

    RingBuffer<Packet, 1024> _incoming;
    RingBuffer<Packet, 1024> _outgoing;

    std::unordered_map<uint32_t, ClientInfo> _clients;

    void recvLoop();
    void sendLoop();
    void processLoop();
    void handlePacket(const char* data, size_t length, const sockaddr_in& clientAddr);
};

template<typename T>
void UDPServer::queueMessage(const T& msg, const sockaddr_in& addr)
{
    Packet pkt;
    pkt.addr = addr;
    pkt.length = sizeof(T);

    static_assert(sizeof(T) <= pkt.data.size(), "Packet too large!");

    std::memcpy(pkt.data.data(), &msg, sizeof(T));
    _outgoing.push(pkt);
}

#endif /* !UDPSERVER_HPP_ */
