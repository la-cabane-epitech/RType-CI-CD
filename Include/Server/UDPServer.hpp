/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPServer
*/

#ifndef UDPSERVER_HPP_
    #define UDPSERVER_HPP_
    #include <cstdint>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <iostream>
    #include <thread>
    #include <cstring>
    #include <array>
    #include <unordered_map>
    #include <unistd.h>

    #include "Server/RingBuffer.hpp"
    #include "Server/ProtocoleUDP.hpp"

class UDPServer {
    public:
        UDPServer(int port);
        ~UDPServer();

        void start();
        void stop();

    protected:
    private:
        int _sockfd;
        bool _running;

        std::thread _recvThread;
        std::thread _sendThread;


        RingBuffer<Packet, 1024> _incoming;
        RingBuffer<Packet, 1024> _outgoing;

        std::unordered_map<uint32_t, ClientInfo> _clients;

        void recvLoop();
        void sendLoop();
        void handlePacket(const char* data, size_t length, const sockaddr_in& clientAddr);
};

#endif /* !UDPSERVER_HPP_ */
