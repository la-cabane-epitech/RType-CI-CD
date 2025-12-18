/*
** EPITECH PROJECT, 2024
** B-OOP-400-RUN-4-1-raytracer-xavier.huet
** File description:
** Exception.hpp
*/

#ifndef EXCEPTION_HPP
    #define EXCEPTION_HPP
    #include <exception>
    #include <string>

/**
 * @file Exception.hpp
 * @brief Custom exception class definition.
 */

namespace RType {

    /**
     * @class Exception
     * @brief Custom exception class for the arcade project.
     *
     * Inherits from std::exception and allows throwing exceptions with
     * a custom message.
     */
    class Exception : public std::exception {
        public:
            /**
             * @brief Constructs the Exception with a custom message.
             * @param message The error message to associate with the exception.
             */
            explicit Exception(const std::string &message) {
                _message = message;
            }

            /**
             * @brief Returns the error message.
             * @return The message as a C-style string.
             */
            const char *what() const noexcept override {
                return _message.c_str();
            }
        private:
            std::string _message; ///< Exception message
    };
}

#endif