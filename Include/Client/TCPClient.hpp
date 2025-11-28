#ifndef TCPCLIENT_HPP_
#define TCPCLIENT_HPP_

// -------- PLATFORM ABSTRACTION --------
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    using socklen_t = int;
    using SOCKET_FD = SOCKET;
    #define CLOSESOCK closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    using SOCKET_FD = int;
    #define CLOSESOCK close
#endif

#include <string>
#include <stdint.h>
#include <cstring>
#include "Protocole/ProtocoleTCP.hpp"

class TCPClient {
private:
    SOCKET_FD _sock;
    std::string _serverIp;
    uint16_t _port;

public:
    TCPClient(const std::string& serverIp, uint16_t port);
    ~TCPClient();

    bool connectToServer();
    bool sendConnectRequest(const std::string& username, ConnectResponse& outResponse);
};

#endif // TCPCLIENT_HPP_
