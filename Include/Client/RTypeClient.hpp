/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** RTypeClient.hpp
*/

#ifndef RTYPECLIENT_HPP_
#define RTYPECLIENT_HPP_

#include "./UDPClient.hpp"
#include "./GameState.hpp"
#include "./Renderer.hpp"

class RTypeClient {
    private:
        UDPClient _network;
        GameState _gamestate;
        Renderer _renderer;
    public:
        run();
        handleInput();
        update();
        render();

};

#endif // RTYPECLIENT_HPP_