/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ParallaxLayer.hpp
*/

#ifndef PARALLAXLAYER_HPP_
#define PARALLAXLAYER_HPP_
    #include "raylib.h"
    #include <vector>

/**
 * @file ParallaxLayer.hpp
 * @brief Defines a single layer for a parallax scrolling background.
 */

/**
 * @class ParallaxLayer
 * @brief Manages the state and rendering of a single parallax background layer.
 */
class ParallaxLayer {
public:
    /**
     * @brief Construct a new Parallax Layer object.
     * @param speed The scrolling speed of the layer relative to the camera.
     * @param texture The texture to use for the layer.
     * @param scale The scale at which to draw the texture.
     * @param y The vertical position of the layer.
     * @param initialX The initial horizontal offset.
     */
    ParallaxLayer(float speed, Texture2D texture, float scale, float y = 0.0f, float initialX = 0.0f);
    /**
     * @brief Destroy the Parallax Layer object.
     */
    ~ParallaxLayer();
    /**
     * @brief Updates the layer's offset based on time and camera speed.
     * @param dt Delta time since the last frame.
     * @param cameraSpeed The speed at which the camera is moving.
     */
    void update(float dt, float cameraSpeed);
    /**
     * @brief Draws the layer to the screen.
     * Draws the texture twice to create a seamless scrolling effect.
     */
    void draw();
private:
    float _speed;       ///< Scrolling speed relative to the camera.
    float _offset;      ///< Current horizontal offset of the texture.
    Texture2D _texture; ///< The texture for this layer.
    float _scale;       ///< The scale of the texture.
    float _y;           ///< The vertical position of the layer.
    float _initialX;    ///< The initial horizontal offset.
};

#endif // PARALLAXLAYER_HPP_
