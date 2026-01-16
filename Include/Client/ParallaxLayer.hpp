/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ParallaxLayer.hpp
*/

#ifndef BACKGROUND_HPP_
#define BACKGROUND_HPP_
    #include "raylib.h"
    #include <vector>

/**
 * @file ParallaxLayer.hpp
 * @brief Header file for the ParallaxLayer class.
 */

/**
 * @class ParallaxLayer
 * @brief Represents a single layer in a parallax background.
 *
 * This class handles the rendering and updating of a scrolling background layer.
 * It supports infinite scrolling by repeating the texture.
 */
class ParallaxLayer {
public:
    /**
     * @brief Construct a new ParallaxLayer object.
     * @param speed The speed multiplier for this layer relative to the camera speed.
     * @param texture The texture to use for this layer.
     * @param scale The scale factor to apply to the texture.
     * @param y The vertical position of the layer (default 0.0f).
     * @param initialX The initial horizontal offset (default 0.0f).
     */
    ParallaxLayer(float speed, Texture2D texture, float scale, float y = 0.0f, float initialX = 0.0f);

    /**
     * @brief Destroy the ParallaxLayer object.
     */
    ~ParallaxLayer();

    /**
     * @brief Updates the position of the layer based on time and camera movement.
     * @param dt Delta time since the last frame.
     * @param cameraSpeed The base speed of the camera/game scroll.
     */
    void update(float dt, float cameraSpeed);

    /**
     * @brief Draws the layer to the screen.
     * Renders the texture twice to create a seamless scrolling effect.
     */
    void draw();
private:
    float _speed; /**< Speed multiplier for the parallax effect. */
    float _offset; /**< Current horizontal offset of the layer. */
    Texture2D _texture; /**< The texture used for the layer. */
    float _scale; /**< Scale factor for the texture. */
    float _y; /**< Vertical position of the layer. */
    float _initialX; /**< Initial horizontal offset. */
};

#endif // BACKGROUND_HPP_
