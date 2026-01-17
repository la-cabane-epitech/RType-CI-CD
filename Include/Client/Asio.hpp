/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** Asio.hpp
*/

#ifndef ASIO_HPP_
#define ASIO_HPP_

#ifdef WIN32
    #include "Client/Windows.hpp"
#endif

/**
 * @file Asio.hpp
 * @brief Wrapper for Asio inclusion to handle platform-specific dependencies.
 * This file ensures that on Windows, the necessary headers are included before Asio.
 */

#include <asio.hpp>

#endif /* !ASIO_HPP_ */
