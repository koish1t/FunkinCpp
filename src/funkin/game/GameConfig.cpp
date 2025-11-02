#include "GameConfig.h"
#include <fstream>

GameConfig* GameConfig::instance = nullptr;

GameConfig::GameConfig() {
    loadConfig();
}

GameConfig* GameConfig::getInstance() {
    if (!instance) {
        instance = new GameConfig();
    }
    return instance;
}

void GameConfig::loadConfig() {
    std::ifstream file("assets/data/config.json");
    if (!file.is_open()) {
        std::cerr << "Could not open config.json" << std::endl;
        return;
    }

    try {
        file >> config;
        
        if (config.contains("gameConfig")) {
            auto& gameConfig = config["gameConfig"];
            downscroll = gameConfig.value("downscroll", false);
            ghostTapping = gameConfig.value("ghostTapping", false);
        } else {
            downscroll = false;
            ghostTapping = false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing config.json: " << e.what() << std::endl;
    }
}

void GameConfig::setDownscroll(bool value) {
    downscroll = value;
    config["gameConfig"]["downscroll"] = value;
    saveConfig();
}

void GameConfig::setGhostTapping(bool value) {
    ghostTapping = value;
    config["gameConfig"]["ghostTapping"] = value;
    saveConfig();
}

void GameConfig::saveConfig() {
    std::ofstream file("assets/data/config.json");
    if (!file.is_open()) {
        std::cerr << "Could not open config.json for writing" << std::endl;
        return;
    }

    try {
        file << config.dump(4);
    } catch (const std::exception& e) {
        std::cerr << "Error saving config.json: " << e.what() << std::endl;
    }
}
