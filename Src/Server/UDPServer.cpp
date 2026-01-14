/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPServer
*/
#include <fcntl.h> // Pour fcntl (systèmes Unix-like)
#ifdef _WIN32
#include <winsock2.h> // Pour ioctlsocket (Windows)
#endif

#include "Server/UDPServer.hpp"
#include "Server/Game.hpp"

UDPServer::UDPServer(int port, std::map<int, std::shared_ptr<Game>>& rooms, Clock& clock)
    : _rooms(rooms), _clock(clock)
{
    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (_sockfd < 0)
        throw std::runtime_error("Failed to create UDP socket");

    #ifdef _WIN32
        u_long mode = 1;
        ioctlsocket(_sockfd, FIONBIO, &mode);
    #else
        fcntl(_sockfd, F_SETFL, O_NONBLOCK);
    #endif

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
                for (auto& [id, game] : _rooms) {
                    if (game->getPlayer(p->playerId)) {
                        game->updatePlayerUdpAddr(p->playerId, clientAddr);
                        game->setPlayerLastProcessedTick(p->playerId, p->tick);

                        if (Player* player = game->getPlayer(p->playerId)) {
                            if (p->inputs & UP) player->y -= player->velocity;
                            if (p->inputs & DOWN) player->y += player->velocity;
                            if (p->inputs & LEFT) player->x -= player->velocity;
                            if (p->inputs & RIGHT) player->x += player->velocity;
                            if (p->inputs & SHOOT) game->createPlayerShot(p->playerId, *this);
                            if (p->inputs & CHARGE_SHOOT) game->createPlayerChargedShot(p->playerId, *this);
                        }
                        break;
                    }
                }
            }
            break;
        case PLAYER_DISCONNECT:
            if (length == sizeof(PlayerDisconnectPacket)) {
                const auto* p = reinterpret_cast<const PlayerDisconnectPacket*>(data);
                for (auto& [id, game] : _rooms) {
                    if (game->getPlayer(p->playerId)) {
                        game->disconnectPlayer(p->playerId, *this);
                        break;
                    }
                }
            }
            break;
        case PING:
            if (length == sizeof(PingPacket)) {
                const auto* p = reinterpret_cast<const PingPacket*>(data);
                PongPacket pongPkt{ .type = PONG, .timestamp = p->timestamp };
                queueMessage(pongPkt, clientAddr);
            }
            break;
    }
}

// Nouvelle surcharge pour envoyer des paquets de données brutes (taille variable)
void UDPServer::queueMessage(const char* data, size_t length, const sockaddr_in& clientAddr)
{
    if (length > MAX_UDP_PACKET_SIZE) {
        std::cerr << "Warning: UDP packet too large (" << length << " bytes), max is " << MAX_UDP_PACKET_SIZE << ". Truncating." << std::endl;
        length = MAX_UDP_PACKET_SIZE; // Tronque si le paquet est trop grand. Une meilleure gestion serait la fragmentation.
    }
    Packet pkt;
    pkt.addr = clientAddr;
    pkt.length = length;
    std::memcpy(pkt.data.data(), data, length);
    _outgoing.push(pkt);
}

