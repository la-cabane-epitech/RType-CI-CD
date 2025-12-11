/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** PlayerState.hpp
*/

#ifndef PLAYERSTATE_HPP_
#define PLAYERSTATE_HPP_

#include <SFML/Graphics.hpp>
#include <map>
#include <cstdint>

enum class PlayerState {
    IDLE,
    UP,
    DOWN,
    SHOOT
};

class Player {
    public:
        Player();
        void setState(PlayerState state);
        void draw(sf::RenderWindow& window);
        void setPosition(float x, float y);
        sf::Sprite& getSprite();
        PlayerState getState();

    private:
        sf::Texture _texture;
        sf::Sprite _sprite;
        PlayerState _state;
        std::map<PlayerState, sf::IntRect> _spriteRects;
};

#endif