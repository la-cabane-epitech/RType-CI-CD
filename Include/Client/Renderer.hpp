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

/**
 * @file Renderer.hpp
 * @brief Header file for the Renderer class.
 */

/**
 * @struct Star
 * @brief Represents a star in the background parallax effect.
 */
struct Star {
    float x;     /**< X coordinate */
    float y;     /**< Y coordinate */
    float speed; /**< Movement speed */
    float scale; /**< Size scale */
};

/**
 * @struct EntityRenderConfig
 * @brief Configuration for rendering a specific entity type.
 */
struct EntityRenderConfig {
    int textureId;      /**< ID of the loaded texture */
    bool isAnimated;    /**< Uses a spritesheet? */
    int frameCount;     /**< Number of animation frames */
    float frameSpeed;   /**< Speed (frames per second) */
    float width;        /**< Width of a frame (or 0 for the entire texture) */
    float height;       /**< Height of a frame */
    float scale;        /**< Rendering scale */
    float startX;       /**< Starting X position in the texture */
    float startY;       /**< Starting Y position in the texture */
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
    void draw();

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
     * @brief Gets the string representation of a Raylib key code.
     * @param key The Raylib key code.
     * @return A C-string representing the key name.
     */
    const char* GetKeyName(int key);

    /**
     * @brief Registry mapping entity types to their rendering configurations.
     */
    std::map<int, EntityRenderConfig> ENTITY_REGISTRY;


private:
    GameState& _gameState; /**< Reference to the shared game state. */
    std::map<uint16_t, Texture2D> _textures; /**< Map of loaded textures. */
    std::vector<ParallaxLayer> _parallaxLayers; /**< Layers for the parallax background. */
    std::optional<std::string> _actionToRemap; /**< The action currently being remapped in the options menu. */
};

#endif