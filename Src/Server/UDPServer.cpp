/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPServer
*/

#include "Server/UDPServer.hpp"

UDPServer::UDPServer(int port)
{
    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sockfd < 0)
        throw std::runtime_error("Failed to create UDP socket");

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_sockfd, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Failed to bind UDP socket");
    _running = false;
}

UDPServer::~UDPServer()
{
    stop();
}

void UDPServer::start()
{
    if (_running)
        return;
    _running = true;

    std::cout << "[UDP] Server starting..." << std::endl;
    _recvThread = std::thread(&UDPServer::recvLoop, this);
    _sendThread = std::thread(&UDPServer::sendLoop, this);
    _processThread = std::thread(&UDPServer::processLoop, this);
}

void UDPServer::stop()
{
    if (!_running)
        return;
    _running = false;

    std::cout << "[UDP] Server stoping..." << std::endl;
    close(_sockfd);
    if (_recvThread.joinable())
        _recvThread.join();
    if (_sendThread.joinable())
        _sendThread.join();
    if (_processThread.joinable())
        _processThread.join();
}

void UDPServer::recvLoop()
{
    while (_running) {
        Packet pkt {};
        sockaddr_in clientAddr {};
        socklen_t addrLen = sizeof(clientAddr);
        ssize_t bytesReceived = recvfrom(_sockfd, pkt.data.data(), pkt.data.size(), 0,
                                         (sockaddr *)&clientAddr, &addrLen);
        if (bytesReceived > 0) {
            pkt.addr = clientAddr;
            pkt.length = static_cast<size_t>(bytesReceived);
            _incoming.push(pkt);
        }
    }
}

void UDPServer::sendLoop()
{
    while (_running) {
        auto pktOpt = _outgoing.pop();
        if (pktOpt) {
            const Packet& pkt = *pktOpt;
            sendto(_sockfd, pkt.data.data(), pkt.length, 0,
                   (sockaddr *)&pkt.addr, sizeof(pkt.addr));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void UDPServer::processLoop()
{
    while (_running) {
        auto pktOpt = _incoming.pop();
        if (pktOpt) {
            const Packet& pkt = *pktOpt;
            handlePacket(pkt.data.data(), pkt.length, pkt.addr);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void UDPServer::handlePacket(const char* data, size_t length, const sockaddr_in& clientAddr)
{
    if (length == sizeof(PlayerInputPacket)) {
        const PlayerInputPacket* clientPacket = reinterpret_cast<const PlayerInputPacket*>(data);

        std::cout << "[UDP] Received PlayerInputPacket from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "\n"
                  << "  - PlayerID: " << clientPacket->playerId << "\n"
                  << "  - Inputs: up=" << ((clientPacket->inputs & UP) ? 1 : 0)
                  << ", down=" << ((clientPacket->inputs & DOWN) ? 1 : 0)
                  << ", left=" << ((clientPacket->inputs & LEFT) ? 1 : 0)
                  << ", right=" << ((clientPacket->inputs & RIGHT) ? 1 : 0)
                  << ", shoot=" << ((clientPacket->inputs & SHOOT) ? 1 : 0) << std::endl;
    }
}
