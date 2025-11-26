/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPServer
*/

#include "Server/TCPServer.hpp"
#include "Server/Utils.hpp"
using namespace NetworkUtils;

TCPServer::TCPServer(int port)
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_sockfd, (sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Failed to bind TCP socket");
    if (listen(_sockfd, 10) < 0)
        throw std::runtime_error("Failed to listen on TCP socket");

    _running = false;
}

TCPServer::~TCPServer()
{
    stop();
}

void TCPServer::start()
{
    if (_running)
        return;
    _running = true;

    std::cout << "[TCP] Server starting..." << std::endl;
    _acceptThread = std::thread(&TCPServer::acceptLoop, this);
}

void TCPServer::stop()
{
    if (!_running)
        return;
    _running = false;

    std::cout << "[TCP] Server stoping..." << std::endl;
    close(_sockfd);
    if (_acceptThread.joinable())
        _acceptThread.join();
    for (auto& thread : _clientThread) {
        if (thread.joinable())
            thread.join();
    }
}

void TCPServer::acceptLoop()
{
    while (_running) {
        int clientSock = accept(_sockfd, nullptr, nullptr);
        std::cout << "[TCP] Client connection..." << std::endl;
        if (clientSock >= 0) {
            _clientThread.emplace_back(&TCPServer::handleClient, this, clientSock);
            _clientThread.back().detach();
        }
    }
}

void TCPServer::handleClient(int clientSock)
{
    ConnectRequest req {};
    // Ce qu'envoie le client sur la socket
    if (!recvAll(clientSock, &req, sizeof(req))) {
        std::cerr << "[TCP] Failed to receive ConnectRequest" << std::endl;
        close(clientSock);
        return;
    }
    std::cout << "[TCP] Type request = " << static_cast<int>(req.type) << std::endl;
    std::cout << "[TCP] Username = " << req.username << std::endl;

    // Check si le client veut se connecter
    if (req.type != 1) {
        ErrorResponse err {};
        err.type = 3;
        strncpy(err.message, "Invalid request type", sizeof(err.message) - 1);
        err.message[sizeof(err.message)-1] = '\0';
        sendAll(clientSock, &err, sizeof(err));
        close(clientSock);
        return;
    }

    // Attribuer un ID joueur et définir un port UDP (port a modif ptet)
    ConnectResponse res {};
    res.type = 2;
    res.playerId = _nextPlayerId++;
    res.udpPort = 5252;
    
    // Le serveur ecrit sur la socket du client
    if (!sendAll(clientSock, &res, sizeof(res))) {
        std::cerr << "[TCP] Failed to send ConnectResponse" << std::endl;
    }
    std::cout << "[TCP] Type response = " << static_cast<int>(res.type)<< std::endl;
    std::cout << "[TCP] PlayerId = " << res.playerId << std::endl;
    std::cout << "[TCP] Use UDP port = " << res.udpPort << std::endl;

    close(clientSock);
}
