/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** TCPServer
*/

#include "Server/TCPServer.hpp"
#include "Network/Utils.hpp"
using namespace NetworkUtils;

TCPServer::TCPServer(int port)
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(_sockfd, (sockaddr *)&addr, sizeof(addr));
    listen(_sockfd, 10);
}

TCPServer::~TCPServer()
{
}

void TCPServer::start()
{
    _running = true;
    std::cout << "[TCP] Server starting..." << std::endl;
    std::thread(&TCPServer::acceptLoop, this).detach();
}

void TCPServer::stop()
{
    _running = false;
    std::cout << "[TCP] Server stoping..." << std::endl;
    close(_sockfd);
}

void TCPServer::acceptLoop()
{
    while (_running) {
        int clientSock = accept(_sockfd, nullptr, nullptr);
        std::cout << "[TCP] Client connection..." << std::endl;
        if (clientSock >= 0)
            std::thread(&TCPServer::handleClient, this, clientSock).detach();
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
    res.udpPort = 50000 + (res.playerId % 10);
    
    // Le serveur ecrit sur la socket du client
    if (!sendAll(clientSock, &res, sizeof(res))) {
        std::cerr << "[TCP] Failed to send ConnectResponse" << std::endl;
    }
    std::cout << "[TCP] Type response = " << static_cast<int>(res.type)<< std::endl;
    std::cout << "[TCP] PlayerId = " << res.playerId << std::endl;
    std::cout << "[TCP] Use UDP port = " << res.udpPort << std::endl;

    close(clientSock);
}
