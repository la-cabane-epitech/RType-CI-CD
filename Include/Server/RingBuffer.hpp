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

template<typename T, size_t Capacity = 1024>
class RingBuffer {
    public:
        RingBuffer() : _head(0), _tail(0), _count(0) {};
        ~RingBuffer() = default;

        bool push(const T& item) {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_count == Capacity) {
                return false; // Buffer is full
            }
            _buffer[_head] = item;
            _head = (_head + 1) % Capacity;
            ++_count;
            return true;
        }

        std::optional<T> pop() {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_count == 0) {
                return std::nullopt; // Buffer is empty
            }
            T item = _buffer[_tail];
            _tail = (_tail + 1) % Capacity;
            --_count;
            return item;
        }

        bool isEmpty() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _count == 0;
        }
        bool isFull() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _count == Capacity;
        }

        size_t count() {
            std::lock_guard<std::mutex> lock(_mutex);
            return _count;
        }

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
