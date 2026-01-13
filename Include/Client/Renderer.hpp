/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Renderer.hpp
*/

#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "GameState.hpp"
#include "ParallaxLayer.hpp"
#include <map>
#include <vector>
#include "Client/TCPClient.hpp"
#include "Protocole/ProtocoleTCP.hpp"
#include "Client/Ray.hpp"

/**
 * @class Renderer
 * @brief Responsible for rendering the game state to the screen.
 *
 * Uses Raylib to draw players and entities based on the current GameState.
 */
class Renderer {
public:
    /**
     * @brief Construct a new Renderer object.
     * @param gameState Reference to the shared GameState object.
     */
    Renderer(GameState& gameState);

    /**
     * @brief Destroy the Renderer object.
     */
    ~Renderer();

    /**
     * @brief Draws the current frame based on the game state.
     */
    void draw();

    /**
     * @brief Draws the room selection menu.
     * @param rooms List of available rooms.
     * @return int -1 if no action, -2 for create room, >= 0 for joined room ID.
     */
    int drawRoomMenu(const std::vector<RoomInfo>& rooms);

    /**
     * @brief Draws the lobby screen.
     * @param lobbyState The current state of the lobby.
     * @param myPlayerId The ID of the local player.
     * @return true if the "Start Game" button was pressed, false otherwise.
     */
    bool drawLobby(const LobbyState& lobbyState, uint32_t myPlayerId);

private:
    GameState& _gameState;
    std::map<uint16_t, Texture2D> _textures;
    std::vector<ParallaxLayer> _parallaxLayers;
};

#endif