/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Patterns.hpp
*/

#pragma once

#include <cmath>
#include <memory>
#include <functional>

// Interface for movement patterns (Strategy Pattern)
class IMovementPattern {
public:
    virtual ~IMovementPattern() = default;

    // Update position (x, y) based on delta time (dt) in seconds
    virtual void update(float& x, float& y, float dt) = 0;
};

// Pattern 1: Linear movement (e.g., moving left)
class LinearPattern : public IMovementPattern {
public:
    LinearPattern(float speedX, float speedY = 0.0f) 
        : _speedX(speedX), _speedY(speedY) {}

    void update(float& x, float& y, float dt) override {
        x += _speedX * dt;
        y += _speedY * dt;
    }

private:
    float _speedX;
    float _speedY;
};

// Pattern 2: Sinusoidal movement (Wave)
class SinusoidalPattern : public IMovementPattern {
public:
    // speed: horizontal speed
    // amplitude: height of the wave
    // frequency: how fast the wave oscillates
    // startY: the Y axis center of the wave
    SinusoidalPattern(float speed, float amplitude, float frequency, float startY, float initialTime = 0.0f)
        : _speed(speed), _amplitude(amplitude), _frequency(frequency), _startY(startY), _time(initialTime) {}

    void update(float& x, float& y, float dt) override {
        _time += dt;
        x -= _speed * dt; // Move left
        y = _startY + std::sin(_time * _frequency) * _amplitude;
    }

private:
    float _speed;
    float _amplitude;
    float _frequency;
    float _startY;
    float _time;
};

// You can add more patterns here (e.g., ZigZag, Circle, Homing)