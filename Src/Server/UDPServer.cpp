/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** UDPServer
*/
#include "Server/UDPServer.hpp"

UDPServer::UDPServer(int port, INetworkHandler* handler, Clock& clock)
    : _io_context(),
        _socket(_io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
        _handler(handler),
        _clock(clock),
        _running(false)
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
    if (_handler) {
        _handler->onMessageReceived(data, length, clientAddr);
    }
}

void UDPServer::queueMessage(const char* data, size_t length, const sockaddr_in& clientAddr)
{
    if (length > MAX_UDP_PACKET_SIZE) {
        std::cerr << "Warning: UDP packet too large (" << length << " bytes), max is " << MAX_UDP_PACKET_SIZE << ". Truncating." << std::endl;
        length = MAX_UDP_PACKET_SIZE;
    }
    Packet pkt;
    pkt.addr = clientAddr;
    pkt.length = length;
    std::memcpy(pkt.data.data(), data, length);
    _outgoing.push(pkt);
}
