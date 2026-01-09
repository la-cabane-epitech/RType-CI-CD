#ifndef CONFIGMANAGER_HPP_
#define CONFIGMANAGER_HPP_

#include <string>
#include <map>

struct Config {
    std::string username;
    std::map<std::string, int> keybinds;
};

class ConfigManager {
public:
    static void saveConfig(const Config& config, const std::string& filename);
    static Config loadConfig(const std::string& filename);
};

#endif // CONFIGMANAGER_HPP_