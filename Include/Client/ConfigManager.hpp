/*
** EPITECH PROJECT, 2025
** RType-CI-CD
** File description:
** ConfigManager.hpp
*/

#ifndef CONFIGMANAGER_HPP_
#define CONFIGMANAGER_HPP_

#include <string>
#include <map>

/**
 * @file ConfigManager.hpp
 * @brief Manages loading and saving of client configuration, such as username and keybinds.
 */

/**
 * @struct Config
 * @brief Holds the client's configuration data.
 */
struct Config {
    std::string username; ///< The player's username.
    std::map<std::string, int> keybinds; ///< A map of game actions to Raylib key codes.
};

/**
 * @class ConfigManager
 * @brief A static class for handling configuration file operations.
 */
class ConfigManager {
public:
    /**
     * @brief Saves the provided configuration to a file.
     * @param config The configuration object to save.
     * @param filename The path to the configuration file.
     */
    static void saveConfig(const Config& config, const std::string& filename);
    /**
     * @brief Loads the configuration from a file.
     * If the file doesn't exist, it returns a default configuration.
     * @param filename The path to the configuration file.
     * @return The loaded or default configuration object.
     */
    static Config loadConfig(const std::string& filename);
};

#endif // CONFIGMANAGER_HPP_