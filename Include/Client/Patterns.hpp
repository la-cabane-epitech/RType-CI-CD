/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Patterns.hpp
*/

#ifndef PATTERNS_HPP_
#define PATTERNS_HPP_

#include <cmath>
#include <memory>
#include <functional>

/**
 * @file Patterns.hpp
 * @brief Defines movement patterns for game entities using the Strategy Pattern.
 */

/**
 * @class IMovementPattern
 * @brief Interface for movement patterns (Strategy Pattern).
 *
 * Defines the contract for updating an entity's position based on a specific movement logic.
 */
class IMovementPattern {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IMovementPattern() = default;

    /**
     * @brief Updates the position (x, y) based on delta time.
     * @param x Reference to the X coordinate.
     * @param y Reference to the Y coordinate.
     * @param dt Delta time in seconds.
     */
    virtual void update(float& x, float& y, float dt) = 0;
};

/**
 * @class LinearPattern
 * @brief Implements a simple linear movement pattern.
 */
class LinearPattern : public IMovementPattern {
public:
    /**
     * @brief Construct a new LinearPattern object.
     * @param speedX Horizontal speed (pixels per second).
     * @param speedY Vertical speed (pixels per second).
     */
    LinearPattern(float speedX, float speedY = 0.0f)
        : _speedX(speedX), _speedY(speedY) {}

    /**
     * @brief Updates position linearly.
     * @param x Reference to X coordinate.
     * @param y Reference to Y coordinate.
     * @param dt Delta time.
     */
    void update(float& x, float& y, float dt) override {
        x += _speedX * dt;
        y += _speedY * dt;
    }

private:
    float _speedX; /**< Horizontal speed. */
    float _speedY; /**< Vertical speed. */
};

/**
 * @class SinusoidalPattern
 * @brief Implements a sinusoidal (wave) movement pattern.
 */
class SinusoidalPattern : public IMovementPattern {
public:
    /**
     * @brief Construct a new SinusoidalPattern object.
     * @param speed Horizontal speed (subtracted from X, so positive moves left).
     * @param amplitude Height of the wave.
     * @param frequency How fast the wave oscillates.
     * @param startY The Y axis center of the wave.
     * @param initialTime Initial time offset for the wave phase.
     */
    SinusoidalPattern(float speed, float amplitude, float frequency, float startY, float initialTime = 0.0f)
        : _speed(speed), _amplitude(amplitude), _frequency(frequency), _startY(startY), _time(initialTime) {}

    /**
     * @brief Updates position following a sine wave.
     * @param x Reference to X coordinate.
     * @param y Reference to Y coordinate.
     * @param dt Delta time.
     */
    void update(float& x, float& y, float dt) override {
        _time += dt;
        x -= _speed * dt; // Move left
        y = _startY + std::sin(_time * _frequency) * _amplitude;
    }

private:
    float _speed;     /**< Horizontal speed. */
    float _amplitude; /**< Wave amplitude. */
    float _frequency; /**< Wave frequency. */
    float _startY;    /**< Center Y position. */
    float _time;      /**< Accumulated time for sine calculation. */
};

#endif /* !PATTERNS_HPP_ */