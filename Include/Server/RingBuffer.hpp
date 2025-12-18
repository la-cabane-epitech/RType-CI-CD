/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** RingBuffer
*/

#ifndef RINGBUFFER_HPP_
#define RINGBUFFER_HPP_

#include <array>
#include <mutex>
#include <optional>

/**
 * @file RingBuffer.hpp
 * @brief Thread-safe circular buffer implementation.
 */

/**
 * @class RingBuffer
 * @brief A thread-safe fixed-size circular buffer.
 * @tparam T Type of elements stored.
 * @tparam Capacity Maximum number of elements (default 1024).
 */
template<typename T, size_t Capacity = 1024>
class RingBuffer {
    public:
        RingBuffer() : _head(0), _tail(0), _count(0) {};
        ~RingBuffer() = default;

        /**
         * @brief Pushes an item into the buffer.
         * @param item The item to add.
         * @return true if added successfully, false if the buffer is full.
         */
        bool push(const T& item) {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_count == Capacity) {
                return false;
            }
            _buffer[_head] = item;
            _head = (_head + 1) % Capacity;
            ++_count;
            return true;
        }

        /**
         * @brief Pops an item from the buffer.
         * @return std::optional<T> The item if available, or std::nullopt if empty.
         */
        std::optional<T> pop() {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_count == 0) {
                return std::nullopt;
            }
            T item = _buffer[_tail];
            _tail = (_tail + 1) % Capacity;
            --_count;
            return item;
        }

        /**
         * @brief Checks if the buffer is empty.
         * @return true if empty, false otherwise.
         */
        bool isEmpty() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _count == 0;
        }

        /**
         * @brief Checks if the buffer is full.
         * @return true if full, false otherwise.
         */
        bool isFull() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _count == Capacity;
        }

        /**
         * @brief Returns the current number of elements in the buffer.
         * @return size_t Number of elements.
         */
        size_t count() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _count;
        }

        /**
         * @brief Returns the capacity of the buffer.
         * @return constexpr size_t The fixed capacity.
         */
        static constexpr size_t capacity() {
            return Capacity;
        }
    protected:
    private:
        std::array<T, Capacity> _buffer;
        size_t _head;
        size_t _tail;
        size_t _count;
        std::mutex _mutex;
};

#endif /* !RINGBUFFER_HPP_ */
