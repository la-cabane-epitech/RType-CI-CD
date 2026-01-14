/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPServer
*/

#include "Server/UDPServer.hpp"
#include "Server/Game.hpp"

UDPServer::UDPServer(int port, std::map<int, std::shared_ptr<Game>>& rooms, Clock& clock)
    : _io_context(),
      _socket(_io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
      _running(false),
      _rooms(rooms),
      _clock(clock)
{
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
    _socket.close();
    _io_context.stop();
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
        try {
            Packet pkt{};
            asio::ip::udp::endpoint sender_endpoint;

            pkt.length = _socket.receive_from(
                asio::buffer(pkt.data), sender_endpoint);

            pkt.addr = *reinterpret_cast<const sockaddr_in*>(sender_endpoint.data());
            _incoming.push(pkt);
        } catch (const std::exception& e) {
            if (_running) {
                std::cerr << "[UDP] Recv error: " << e.what() << std::endl;
            }
        }
    }
}

void UDPServer::sendLoop()
{
    while (_running) {
        if (_outgoing.isEmpty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        auto pktOpt = _outgoing.pop();
        if (!pktOpt) continue;
        const Packet& pkt = *pktOpt;

        asio::ip::address_v4::bytes_type addr_bytes;
        std::memcpy(addr_bytes.data(), &pkt.addr.sin_addr.s_addr, 4);
        asio::ip::address_v4 address(addr_bytes);
        unsigned short port = ntohs(pkt.addr.sin_port);
        asio::ip::udp::endpoint destination(address, port);

        asio::error_code ec;
        _socket.send_to(asio::buffer(pkt.data.data(), pkt.length), destination, 0, ec);

        if (ec) {
            std::cerr << "[UDP] Send error to " << destination.address().to_string() << ":" << destination.port() << " - " << ec.message() << std::endl;
        }
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
