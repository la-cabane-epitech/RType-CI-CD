#ifndef TCPCLIENT_HPP_
#define TCPCLIENT_HPP_

    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>

#include <string>
#include <stdint.h>
#include <cstring>
#include "Protocole/ProtocoleTCP.hpp"

class TCPClient {
private:
    int _sock;
    std::string _serverIp;
    uint16_t _port;

public:
    TCPClient(const std::string& serverIp, uint16_t port);
    ~TCPClient();

    bool connectToServer();
    bool sendConnectRequest(const std::string& username, ConnectResponse& outResponse);
};

#endif // TCPCLIENT_HPP_
