/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Clock
*/

#ifndef CLOCK_HPP_
#define CLOCK_HPP_

#include <chrono>
#include <cstdint>

/**
 * @file Clock.hpp
 * @brief Simple high-resolution timer class.
 */

/**
 * @class Clock
 * @brief A simple high-resolution clock for measuring elapsed time.
 *
 * This class provides a way to get the time elapsed since its creation,
 * typically the start of the application. It's useful for game loops,
 * animations, and network timestamps.
 */
class Clock {
public:
    Clock() : _startTime(std::chrono::high_resolution_clock::now()) {}

    /**
     * @brief Returns the time elapsed since the clock was created, in milliseconds.
     * @return uint32_t The elapsed time in milliseconds.
     */
    uint32_t getElapsedTimeMs() const {
        auto now = std::chrono::high_resolution_clock::now();
        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now - _startTime).count());
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> _startTime;
};

#endif /* !CLOCK_HPP_ */