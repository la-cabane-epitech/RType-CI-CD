/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** main
*/

#include <SFML/Graphics.hpp>
#include "Client/PlayerState.hpp"
#include "Client/RTypeClient.hpp"
#include "Client/TCPClient.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "R-Type");
    window.setFramerateLimit(60);

    Player player;
    player.setPosition(100, 300);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Gestion des états
        bool isShooting = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
        bool isMovingUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
        bool isMovingDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

        if (isShooting) {
            player.setState(PlayerState::SHOOT);
        } else if (isMovingUp) {
            player.setState(PlayerState::UP);
            // Ici, vous ajouteriez aussi le code pour déplacer le vaisseau vers le haut
            // player.getSprite().move(0, -5.f);
        } else if (isMovingDown) {
            player.setState(PlayerState::DOWN);
            // player.getSprite().move(0, 5.f);
        } else {
            player.setState(PlayerState::IDLE);
        }

        // Rendu
        window.clear(sf::Color::Black);
        player.draw(window);
        window.display();
    }

    ConnectResponse res;
    if (!tcpClient.sendConnectRequest("Player1", res)) {
        std::cerr << "Handshake TCP échoué\n";
        return 1;
    }

    std::cout << "Connecté ! PlayerId: " << res.playerId
            << ", UDP Port: " << res.udpPort << "\n";

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "R-Type Client");
    SetTargetFPS(60);

    {
        RTypeClient client(serverIp, res);
        client.run();
    }

    CloseWindow();
    // indiquer au server la fermeture + deconnecter
    return 0;
}
