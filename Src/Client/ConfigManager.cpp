#include "Client/ConfigManager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

void ConfigManager::saveConfig(const Config& config, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file for writing: " << filename << std::endl;
        return;
    }

    file << "username=" << config.username << std::endl;
    for (const auto& pair : config.keybinds) {
        file << pair.first << "=" << pair.second << std::endl;
    }
}

Config ConfigManager::loadConfig(const std::string& filename) {
    Config config;
    config.keybinds = {
        {"UP", 87},
        {"DOWN", 83},
        {"LEFT", 65},
        {"RIGHT", 68},
        {"SHOOT", 32}
    };

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Info: No config file found. Using defaults." << std::endl;
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            if (key == "username") {
                config.username = value;
            } else {
                try { config.keybinds[key] = std::stoi(value); } catch (const std::exception&) {}
            }
        }
    }
    return config;
}