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
#include <optional>
#include <string>
#include "Protocole/ProtocoleTCP.hpp"
#include "Client/Ray.hpp"

struct Star {
    float x;
    float y;
    float speed;
    float scale;
};

struct EntityRenderConfig {
    int textureId;      // ID de la texture chargée
    bool isAnimated;    // Utilise une spritesheet ?
    int frameCount;     // Nombre de frames d'animation
    float frameSpeed;   // Vitesse (frames par seconde)
    float width;        // Largeur d'une frame (ou 0 pour toute la texture)
    float height;       // Hauteur d'une frame
    float scale;        // Échelle de rendu
    float startX;       // Position X de départ dans la texture
    float startY;       // Position Y de départ dans la texture
};

/**
 * @enum MainMenuChoice
 * @brief Represents the user's choice in the main menu.
 */
enum class MainMenuChoice {
    NONE,
    START,
    OPTIONS
};

/**
 * @enum PauseMenuChoice
 * @brief Represents the user's choice in the in-game pause menu.
 */
enum class PauseMenuChoice {
    NONE,
    OPTIONS,
    QUIT
};

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
     * @brief Draws the game world content (players, entities, background).
     * @note This function should be called within a BeginDrawing()/EndDrawing() block.
     */
    void draw(const std::map<std::string, int>& keybinds);

    /**
     * @brief Draws the in-game chat overlay.
     * @param messages The history of chat messages.
     * @param currentInput The current text being typed.
     * @param isActive Whether the chat input is active.
     */
    void drawChat(const std::vector<std::string>& messages, const std::string& currentInput, bool isActive);

    /**
     * @brief Draws the pause menu.
     * @return The user's choice from the pause menu.
     */
    PauseMenuChoice drawPauseMenu();

    /**
     * @brief Draws the main menu.
     * @return The user's choice from the main menu.
     */
    MainMenuChoice drawMainMenu();

    /**
     * @brief Draws the options menu for key remapping.
     * @param keybinds A map of action names to Raylib key codes. This map will be modified if the user changes a key.
     * @return true if the user clicked the "Back" button, false otherwise.
     */
    bool drawOptionsMenu(std::map<std::string, int>& keybinds);

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

    /**
     * @brief Draws the username input screen.
     * @param username Reference to the string that will store the username.
     * @return true if the user confirmed the username (pressed Enter), false otherwise.
     */
    bool drawUsernameInput(std::string& username);

    /**
     * @brief Draws the "kicked from server" screen.
     */
    void drawKickedScreen();

    const char* GetKeyName(int key);

    std::map<int, EntityRenderConfig> ENTITY_REGISTRY;


private:
    GameState& _gameState;
    std::map<uint16_t, Texture2D> _textures;
    std::vector<ParallaxLayer> _parallaxLayers;
    std::optional<std::string> _actionToRemap;
    std::map<uint32_t, float> _playerBank;
};

#endif