/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Background.hpp
*/

#ifndef BACKGROUND_HPP_
#define BACKGROUND_HPP_
    #include "raylib.h"
    #include <vector>

class ParallaxLayer {
public:
    ParallaxLayer(float speed, Texture2D texture, float scale, float y = 0.0f, float initialX = 0.0f);
    ~ParallaxLayer();
    void update(float dt, float cameraSpeed);
    void draw();
private:
    float _speed;
    float _offset;
    Texture2D _texture;
    float _scale;
    float _y;
    float _initialX;
};

#endif // BACKGROUND_HPP_
