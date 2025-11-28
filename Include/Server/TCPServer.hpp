#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    using SocketType = SOCKET;
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    using SocketType = int;
#endif

#include <thread>
#include <vector>
#include "Server/Game.hpp"

#include "Protocole/ProtocoleTCP.hpp"
#include "Server/Game.hpp"

class TCPServer {
public:
    TCPServer(int port, Game& game);
    ~TCPServer();

    void start();
    void stop();

private:
    void acceptLoop();
    void handleClient(SocketType clientSock);

    SocketType _sockfd;
    bool _running;
    Game& _game;
    uint32_t _nextPlayerId = 1;

    std::thread _acceptThread;
    std::vector<std::thread> _clientThread;
};

#endif
