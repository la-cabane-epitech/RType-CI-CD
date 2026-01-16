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
 * @brief Header file for configuration management.
 */

/**
 * @struct Config
 * @brief Holds the application configuration data.
 */
struct Config {
    std::string username; /**< The player's username. */
    std::map<std::string, int> keybinds; /**< Map of action names to key codes. */
};

/**
 * @class ConfigManager
 * @brief Static class for loading and saving configuration files.
 */
class ConfigManager {
public:
    /**
     * @brief Saves the current configuration to a file.
     * @param config The configuration object to save.
     * @param filename The path to the destination file.
     */
    static void saveConfig(const Config& config, const std::string& filename);

    /**
     * @brief Loads configuration from a file.
     * @param filename The path to the configuration file.
     * @return A Config object containing the loaded settings, or defaults if the file is missing/invalid.
     */
    static Config loadConfig(const std::string& filename);
};

#endif // CONFIGMANAGER_HPP_