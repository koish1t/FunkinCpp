#include "Controls.h"
#include <flixel/FlxG.h>
#include <fstream>
#include <iostream>
#include "../../../../external/nlohmann/json.hpp"

Controls::Controls() {
    for (int i = 0; i < 4; i++) {
        arrowKeys[i].primary = SDL_SCANCODE_UNKNOWN;
        arrowKeys[i].alternate = SDL_SCANCODE_UNKNOWN;
        nxArrowKeys[i].primary = SDL_CONTROLLER_BUTTON_INVALID;
        nxArrowKeys[i].alternate = SDL_CONTROLLER_BUTTON_INVALID;
    }
}

void Controls::loadFromConfig(const std::string& configPath) {
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config.json" << std::endl;
        return;
    }

    nlohmann::json config;
    try {
        configFile >> config;
    } catch (const std::exception& ex) {
        std::cerr << "Failed to parse config.json: " << ex.what() << std::endl;
        return;
    }

    if (config.contains("mainBinds")) {
        auto mainBinds = config["mainBinds"];
        arrowKeys[0].primary = getScancodeFromString(mainBinds["left"].get<std::string>());
        arrowKeys[1].primary = getScancodeFromString(mainBinds["down"].get<std::string>());
        arrowKeys[2].primary = getScancodeFromString(mainBinds["up"].get<std::string>());
        arrowKeys[3].primary = getScancodeFromString(mainBinds["right"].get<std::string>());
    }

    if (config.contains("altBinds")) {
        auto altBinds = config["altBinds"];
        arrowKeys[0].alternate = getScancodeFromString(altBinds["left"].get<std::string>());
        arrowKeys[1].alternate = getScancodeFromString(altBinds["down"].get<std::string>());
        arrowKeys[2].alternate = getScancodeFromString(altBinds["up"].get<std::string>());
        arrowKeys[3].alternate = getScancodeFromString(altBinds["right"].get<std::string>());
    }

    if (config.contains("nxBinds")) {
        auto nxBinds = config["nxBinds"];
        nxArrowKeys[0].primary = getButtonFromString(nxBinds["left"].get<std::string>());
        nxArrowKeys[1].primary = getButtonFromString(nxBinds["down"].get<std::string>());
        nxArrowKeys[2].primary = getButtonFromString(nxBinds["up"].get<std::string>());
        nxArrowKeys[3].primary = getButtonFromString(nxBinds["right"].get<std::string>());
    }

    if (config.contains("nxAltBinds")) {
        auto nxAltBinds = config["nxAltBinds"];
        nxArrowKeys[0].alternate = getButtonFromString(nxAltBinds["left"].get<std::string>());
        nxArrowKeys[1].alternate = getButtonFromString(nxAltBinds["down"].get<std::string>());
        nxArrowKeys[2].alternate = getButtonFromString(nxAltBinds["up"].get<std::string>());
        nxArrowKeys[3].alternate = getButtonFromString(nxAltBinds["right"].get<std::string>());
    }
}

bool Controls::justPressed(int direction) const {
    if (direction < 0 || direction >= 4) return false;
    
    bool keyboardPressed = flixel::FlxG::keys.keys[arrowKeys[direction].primary].justPressed() || 
                          flixel::FlxG::keys.keys[arrowKeys[direction].alternate].justPressed();
    
    bool controllerPressed = flixel::FlxG::gamepads.justPressed(nxArrowKeys[direction].primary) ||
                            flixel::FlxG::gamepads.justPressed(nxArrowKeys[direction].alternate);
    
    return keyboardPressed || controllerPressed;
}

bool Controls::pressed(int direction) const {
    if (direction < 0 || direction >= 4) return false;
    
    bool keyboardPressed = flixel::FlxG::keys.keys[arrowKeys[direction].primary].pressed() || 
                          flixel::FlxG::keys.keys[arrowKeys[direction].alternate].pressed();
    
    bool controllerPressed = flixel::FlxG::gamepads.pressed(nxArrowKeys[direction].primary) ||
                            flixel::FlxG::gamepads.pressed(nxArrowKeys[direction].alternate);
    
    return keyboardPressed || controllerPressed;
}

bool Controls::justReleased(int direction) const {
    if (direction < 0 || direction >= 4) return false;
    
    bool keyboardReleased = flixel::FlxG::keys.keys[arrowKeys[direction].primary].justReleased() || 
                           flixel::FlxG::keys.keys[arrowKeys[direction].alternate].justReleased();
    
    bool controllerReleased = flixel::FlxG::gamepads.justReleased(nxArrowKeys[direction].primary) ||
                             flixel::FlxG::gamepads.justReleased(nxArrowKeys[direction].alternate);
    
    return keyboardReleased || controllerReleased;
}

Controls::KeyBind Controls::getKeyBind(int direction) const {
    if (direction >= 0 && direction < 4) {
        return arrowKeys[direction];
    }
    return {SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN};
}

Controls::NXKeyBind Controls::getNXKeyBind(int direction) const {
    if (direction >= 0 && direction < 4) {
        return nxArrowKeys[direction];
    }
    return {SDL_CONTROLLER_BUTTON_INVALID, SDL_CONTROLLER_BUTTON_INVALID};
}

SDL_Scancode Controls::getScancodeFromString(const std::string& keyName) {
    static const std::map<std::string, SDL_Scancode> keyMap = {
        {"A", SDL_SCANCODE_A}, {"B", SDL_SCANCODE_B}, {"C", SDL_SCANCODE_C},
        {"D", SDL_SCANCODE_D}, {"E", SDL_SCANCODE_E}, {"F", SDL_SCANCODE_F},
        {"G", SDL_SCANCODE_G}, {"H", SDL_SCANCODE_H}, {"I", SDL_SCANCODE_I},
        {"J", SDL_SCANCODE_J}, {"K", SDL_SCANCODE_K}, {"L", SDL_SCANCODE_L},
        {"M", SDL_SCANCODE_M}, {"N", SDL_SCANCODE_N}, {"O", SDL_SCANCODE_O},
        {"P", SDL_SCANCODE_P}, {"Q", SDL_SCANCODE_Q}, {"R", SDL_SCANCODE_R},
        {"S", SDL_SCANCODE_S}, {"T", SDL_SCANCODE_T}, {"U", SDL_SCANCODE_U},
        {"V", SDL_SCANCODE_V}, {"W", SDL_SCANCODE_W}, {"X", SDL_SCANCODE_X},
        {"Y", SDL_SCANCODE_Y}, {"Z", SDL_SCANCODE_Z},
        {"ArrowLeft", SDL_SCANCODE_LEFT}, {"ArrowRight", SDL_SCANCODE_RIGHT},
        {"ArrowUp", SDL_SCANCODE_UP}, {"ArrowDown", SDL_SCANCODE_DOWN},
        {"Space", SDL_SCANCODE_SPACE}, {"Enter", SDL_SCANCODE_RETURN},
        {"Escape", SDL_SCANCODE_ESCAPE}, {"Left", SDL_SCANCODE_LEFT},
        {"Right", SDL_SCANCODE_RIGHT}, {"Up", SDL_SCANCODE_UP},
        {"Down", SDL_SCANCODE_DOWN}
    };

    auto it = keyMap.find(keyName);
    if (it != keyMap.end()) {
        return it->second;
    }

    std::cout << "Unknown key name: " << keyName << std::endl;
    return SDL_SCANCODE_UNKNOWN;
}

SDL_GameControllerButton Controls::getButtonFromString(const std::string& buttonName) {
    static const std::map<std::string, SDL_GameControllerButton> buttonMap = {
        {"A", SDL_CONTROLLER_BUTTON_A}, {"B", SDL_CONTROLLER_BUTTON_B},
        {"X", SDL_CONTROLLER_BUTTON_X}, {"Y", SDL_CONTROLLER_BUTTON_Y},
        {"DPAD_LEFT", SDL_CONTROLLER_BUTTON_DPAD_LEFT},
        {"DPAD_RIGHT", SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
        {"DPAD_UP", SDL_CONTROLLER_BUTTON_DPAD_UP},
        {"DPAD_DOWN", SDL_CONTROLLER_BUTTON_DPAD_DOWN},
        {"LEFT_SHOULDER", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"RIGHT_SHOULDER", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"LEFT_TRIGGER", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"RIGHT_TRIGGER", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"ZL", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"ZR", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"LT", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"RT", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"LEFT_STICK", SDL_CONTROLLER_BUTTON_LEFTSTICK},
        {"RIGHT_STICK", SDL_CONTROLLER_BUTTON_RIGHTSTICK},
        {"START", SDL_CONTROLLER_BUTTON_START},
        {"BACK", SDL_CONTROLLER_BUTTON_BACK}
    };

    auto it = buttonMap.find(buttonName);
    if (it != buttonMap.end()) {
        return it->second;
    }

    std::cout << "Unknown button name: " << buttonName << std::endl;
    return SDL_CONTROLLER_BUTTON_INVALID;
}