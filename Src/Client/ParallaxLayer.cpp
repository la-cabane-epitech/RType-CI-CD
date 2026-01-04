/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ParallaxLayer.cpp
*/

#include "Client/ParallaxLayer.hpp"
#include "raylib.h"

void ParallaxLayer::draw() {
    // On dessine l'image deux fois pour l'effet de boucle infinie
    DrawTextureEx(_texture, {_offset, _y}, 0.0f, _scale, WHITE);
    DrawTextureEx(_texture, {_offset + _texture.width * _scale, _y}, 0.0f, _scale, WHITE);
}

ParallaxLayer::ParallaxLayer(float speed, Texture2D texture, float scale, float y, float initialX)
    : _speed(speed), _texture(texture), _offset(initialX), _scale(scale), _y(y), _initialX(initialX) {
}

void ParallaxLayer::update(float dt, float cameraSpeed) {
    _offset -= _speed * dt * cameraSpeed;

    // Si c'est un fond d'écran (initialX == 0), on boucle dès que la texture est passée
    if (_initialX == 0.0f) {
        if (_offset <= -_texture.width * _scale) {
            _offset = 0.0f;
        }
    } else {
        // Si c'est un objet (astéroïde), on attend qu'il sorte complètement de l'écran avant de le remettre au début
        if (_offset <= -_texture.width * _scale - GetScreenWidth()) {
            _offset = _initialX;
        }
    }
}

ParallaxLayer::~ParallaxLayer() {
}