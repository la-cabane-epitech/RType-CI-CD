#include <sys/socket.h>
#include <arpa/inet.h>
// #include <unistd.h>
#include <cstring>
#include <iostream>

#include "Network/ProtocoleTCP.hpp"
#include "Network/Utils.hpp"
using namespace NetworkUtils;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv {};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(4242);
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);

    connect(sock, (sockaddr *)&serv, sizeof(serv));

    // Setup du message pour le serveur
    ConnectRequest req {};
    req.type = 1;
    strncpy(req.username, "Player1", 32);

    // Le client ecrit sur la socket du serveur
    send(sock, &req, sizeof(req), 0);
    std::cout << "Type request = " << static_cast<int>(req.type) << std::endl;
    std::cout << "Username = " << req.username << std::endl;

    // Lit le type de message
    uint8_t type;
    recv(sock, &type, 1, 0);

    // Complete le reste, 2 pour OK, 3 pour Error
    if (type == 2) {
        ConnectResponse res {};
        res.type = type;
        recv(sock, ((char*)&res) + 1, sizeof(res) - 1, 0);
        std::cout << "Type = " << (int)res.type << "\n";
        std::cout << "PlayerId = " << res.playerId << "\n";
        std::cout << "UDP Port = " << res.udpPort << "\n";
    } else if (type == 3) {
        ErrorResponse err {};
        err.type = type;
        recv(sock, ((char*)&err) + 1, sizeof(err) - 1, 0);
        std::cerr << "Server Error: " << err.message << std::endl;
    }

    close(sock);
}
