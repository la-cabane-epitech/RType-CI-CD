#ifndef TCPCLIENT_HPP_
#define TCPCLIENT_HPP_

#include <string>
#include <stdint.h>
#include <cstring>
#include "Protocole/ProtocoleTCP.hpp"
#include "asio.hpp"

class TCPClient {
private:
    asio::io_context _io_context;
    asio::ip::tcp::socket _socket;
    std::string _serverIp;
    uint16_t _port;

public:
    TCPClient(const std::string& serverIp, uint16_t port);
    ~TCPClient();

    bool connectToServer();
    bool sendConnectRequest(const std::string& username, ConnectResponse& outResponse);
};

#endif // TCPCLIENT_HPP_
