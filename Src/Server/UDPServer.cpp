/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPServer
*/

#include "Server/UDPServer.hpp"
#include "Server/Game.hpp"

UDPServer::UDPServer(int port, Game& game, Clock& clock)
    : _game(game), _clock(clock)
{
    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (_sockfd < 0)
        throw std::runtime_error("Failed to create UDP socket");

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
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

        int received = recvfrom(
            _sockfd,
            pkt.data.data(),
            pkt.data.size(),
            0,
            (sockaddr*)&clientAddr,
            &addrLen
        );

        if (received > 0) {
            pkt.addr = clientAddr;
            pkt.length = received;
            _incoming.push(pkt);
        }
    }
}

void UDPServer::sendLoop()
{
    while (_running) {
        if (_outgoing.isEmpty())
            continue;

        auto pktOpt = _outgoing.pop();
        const Packet& pkt = *pktOpt;

        sendto(
            _sockfd,
            pkt.data.data(),
            pkt.length,
            0,
            (sockaddr*)&pkt.addr,
            sizeof(pkt.addr)
        );
    }
}

void UDPServer::processLoop()
{
    while (_running) {
        std::cout << "The server is running since " << _clock.getElapsedTimeMs() << " ms.\n";
        if (_incoming.isEmpty())
            continue;
        auto pktOpt = _incoming.pop();

        handlePacket(pktOpt->data.data(), pktOpt->length, pktOpt->addr);
    }
}

void UDPServer::handlePacket(const char* data, size_t length, const sockaddr_in& clientAddr)
{
    if (length < 1) return;

    uint8_t type = *reinterpret_cast<const uint8_t*>(data);

    switch (type) {
        case PLAYER_INPUT:
            if (length == sizeof(PlayerInputPacket)) {
                const auto* p = reinterpret_cast<const PlayerInputPacket*>(data);
                _game.updatePlayerUdpAddr(p->playerId, clientAddr);

                // On met à jour le dernier tick traité pour ce joueur
                _game.setPlayerLastProcessedTick(p->playerId, p->tick);

                if (Player* player = _game.getPlayer(p->playerId)) {
                    if (p->inputs & UP) player->y -= player->velocity;
                    if (p->inputs & DOWN) player->y += player->velocity;
                    if (p->inputs & LEFT) player->x -= player->velocity;
                    if (p->inputs & RIGHT) player->x += player->velocity;
                    if (p->inputs & SHOOT) _game.createPlayerShot(p->playerId, *this);
                }
            }
            break;
        case PLAYER_DISCONNECT:
            if (length == sizeof(PlayerDisconnectPacket)) {
                const auto* p = reinterpret_cast<const PlayerDisconnectPacket*>(data);
                _game.disconnectPlayer(p->playerId, *this);
            }
            break;
        case PING:
            if (length == sizeof(PingPacket)) {
                // sleep(2);
                const auto* p = reinterpret_cast<const PingPacket*>(data);
                PongPacket pongPkt{ .type = PONG, .timestamp = p->timestamp };
                queueMessage(pongPkt, clientAddr);
            }
            break;
    }
}
