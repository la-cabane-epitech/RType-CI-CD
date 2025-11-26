// #include <sys/socket.h>
// #include <arpa/inet.h>
// // #include <unistd.h>
// #include <cstring>
// #include <iostream>

// #include "Server/ProtocoleTCP.hpp"
// #include "Server/Utils.hpp"
// using namespace NetworkUtils;

// int main() {
//     int sock = socket(AF_INET, SOCK_STREAM, 0);

//     sockaddr_in serv {};
//     serv.sin_family = AF_INET;
//     serv.sin_port = htons(4242);
//     inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);

//     connect(sock, (sockaddr *)&serv, sizeof(serv));

//     // Setup du message pour le serveur
//     ConnectRequest req {};
//     req.type = 1;
//     strncpy(req.username, "Player1", 32);

//     // Le client ecrit sur la socket du serveur
//     send(sock, &req, sizeof(req), 0);
//     std::cout << "Type request = " << static_cast<int>(req.type) << std::endl;
//     std::cout << "Username = " << req.username << std::endl;

//     // Lit le type de message
//     uint8_t type;
//     recv(sock, &type, 1, 0);

//     // Complete le reste, 2 pour OK, 3 pour Error
//     if (type == 2) {
//         ConnectResponse res {};
//         res.type = type;
//         recv(sock, ((char*)&res) + 1, sizeof(res) - 1, 0);
//         std::cout << "Type = " << (int)res.type << "\n";
//         std::cout << "PlayerId = " << res.playerId << "\n";
//         std::cout << "UDP Port = " << res.udpPort << "\n";
//     } else if (type == 3) {
//         ErrorResponse err {};
//         err.type = type;
//         recv(sock, ((char*)&err) + 1, sizeof(err) - 1, 0);
//         std::cerr << "Server Error: " << err.message << std::endl;
//     }

//     close(sock);
// }


#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(5252); // port UDP que ton serveur a donné
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);

    // Exemple : envoyer un message simple
    const char* msg = "Hello UDP server!";
    while (1)
        sendto(sock, msg, strlen(msg), 0, (sockaddr*)&serv, sizeof(serv));

    // Recevoir une réponse
    // char buffer[1024]{0};
    // sockaddr_in from{};
    // socklen_t fromLen = sizeof(from);
    // ssize_t received = recvfrom(sock, buffer, sizeof(buffer)-1, 0,
    //                             (sockaddr*)&from, &fromLen);
    // if (received > 0) {
    //     buffer[received] = '\0';
    //     std::cout << "Server replied: " << buffer << std::endl;
    // }

    close(sock);
}
