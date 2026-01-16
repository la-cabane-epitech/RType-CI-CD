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

/**
 * @file Patterns.hpp
 * @brief Defines various movement patterns for game entities using the Strategy Pattern.
 */

/**
 * @class IMovementPattern
 * @brief Interface for movement patterns (Strategy Pattern).
 *
 * This abstract class defines the contract for all movement patterns. Each concrete
 * pattern must implement the `update` method to modify an entity's position over time.
 */
class IMovementPattern {
public:
    /**
     * @brief Virtual destructor for the interface.
     */
    virtual ~IMovementPattern() = default;

    /**
     * @brief Updates the position of an entity based on the pattern's logic.
     * @param x Reference to the entity's X coordinate.
     * @param y Reference to the entity's Y coordinate.
     * @param dt Delta time in seconds since the last frame.
     */
    virtual void update(float& x, float& y, float dt) = 0;
};

/**
 * @class LinearPattern
 * @brief A movement pattern that moves an entity in a straight line.
 */
class LinearPattern : public IMovementPattern {
public:
    /**
     * @brief Construct a new Linear Pattern object.
     * @param speedX The horizontal speed.
     * @param speedY The vertical speed (default is 0).
     */
    LinearPattern(float speedX, float speedY = 0.0f) 
        : _speedX(speedX), _speedY(speedY) {}

    /** @copydoc IMovementPattern::update */
    void update(float& x, float& y, float dt) override {
        x += _speedX * dt;
        y += _speedY * dt;
    }

private:
    float _speedX; ///< Horizontal speed component.
    float _speedY; ///< Vertical speed component.
};

/**
 * @class SinusoidalPattern
 * @brief A movement pattern that moves an entity in a sine wave.
 */
class SinusoidalPattern : public IMovementPattern {
public:
    /**
     * @brief Construct a new Sinusoidal Pattern object.
     * @param speed The base horizontal speed.
     * @param amplitude The height of the wave.
     * @param frequency How fast the wave oscillates.
     * @param startY The Y-axis center of the wave.
     * @param initialTime The starting time offset for the sine function.
     */
    SinusoidalPattern(float speed, float amplitude, float frequency, float startY, float initialTime = 0.0f)
        : _speed(speed), _amplitude(amplitude), _frequency(frequency), _startY(startY), _time(initialTime) {}

    /** @copydoc IMovementPattern::update */
    void update(float& x, float& y, float dt) override {
        _time += dt;
        x -= _speed * dt; // Move left
        y = _startY + std::sin(_time * _frequency) * _amplitude;
    }

private:
    float _speed;      ///< Base horizontal speed.
    float _amplitude;  ///< Height of the wave.
    float _frequency;  ///< Oscillation frequency of the wave.
    float _startY;     ///< The Y-axis center of the wave.
    float _time;       ///< Internal timer for the sine function.
};

// You can add more patterns here (e.g., ZigZag, Circle, Homing)