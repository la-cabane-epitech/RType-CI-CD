/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** PlayerState.cpp
*/

#include "Client/PlayerState.hpp"
#include <iostream>

Player::Player() : _state(PlayerState::IDLE) {
    if (!_texture.loadFromFile("assets/spaceship_sheet.png")) {
        std::cerr << "Error loading texture assets/spaceship_sheet.png" << std::endl;
    }
    _sprite.setTexture(_texture);

    // sf::IntRect(left, top, width, height)
    _spriteRects[PlayerState::IDLE] = sf::IntRect(0, 0, 33, 17);
    _spriteRects[PlayerState::SHOOT] = sf::IntRect(33, 0, 33, 17);
    // ... sprites 3 et 4
    _spriteRects[PlayerState::UP] = sf::IntRect(132, 0, 33, 17);
    _spriteRects[PlayerState::DOWN] = sf::IntRect(165, 0, 33, 17); 

    // On commence avec l'état par défaut
    setState(PlayerState::IDLE);
}

// Met à jour l'état du joueur et change le sprite affiché
void Player::setState(PlayerState newState) {
    // On ne met à jour que si l'état a réellement changé
    if (_state != newState) {
        _state = newState;
        // Applique le rectangle correspondant au nouvel état s'il existe
        if (_spriteRects.count(newState)) {
            _sprite.setTextureRect(_spriteRects.at(newState));
        }
    }
}


// Pour dessiner le joueur dans la fenêtre
void Player::draw(sf::RenderWindow &window) {
    window.draw(_sprite);
}

// Pour mettre à jour la position du joueur
void Player::setPosition(float x, float y) {
    _sprite.setPosition(x, y);
}

sf::Sprite& Player::getSprite() {
    return _sprite;
}

PlayerState Player::getState() {
    return _state;
}