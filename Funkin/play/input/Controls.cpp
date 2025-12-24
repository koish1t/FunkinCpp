#include "Controls.h"
#include <flixel/FlxG.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

Controls::Controls() {
    setDefaultBindings();
}

void Controls::setDefaultBindings() {
    for (int i = 0; i < static_cast<int>(ControlAction::COUNT); i++) {
        keyBinds[i].primary = SDL_SCANCODE_UNKNOWN;
        keyBinds[i].alternate = SDL_SCANCODE_UNKNOWN;
        gamepadBinds[i].primary = SDL_CONTROLLER_BUTTON_INVALID;
        gamepadBinds[i].alternate = SDL_CONTROLLER_BUTTON_INVALID;
    }
    
    keyBinds[static_cast<int>(ControlAction::NOTE_LEFT)].primary = SDL_SCANCODE_A;
    keyBinds[static_cast<int>(ControlAction::NOTE_LEFT)].alternate = SDL_SCANCODE_LEFT;
    keyBinds[static_cast<int>(ControlAction::NOTE_DOWN)].primary = SDL_SCANCODE_S;
    keyBinds[static_cast<int>(ControlAction::NOTE_DOWN)].alternate = SDL_SCANCODE_DOWN;
    keyBinds[static_cast<int>(ControlAction::NOTE_UP)].primary = SDL_SCANCODE_W;
    keyBinds[static_cast<int>(ControlAction::NOTE_UP)].alternate = SDL_SCANCODE_UP;
    keyBinds[static_cast<int>(ControlAction::NOTE_RIGHT)].primary = SDL_SCANCODE_D;
    keyBinds[static_cast<int>(ControlAction::NOTE_RIGHT)].alternate = SDL_SCANCODE_RIGHT;
    
    keyBinds[static_cast<int>(ControlAction::UI_LEFT)].primary = SDL_SCANCODE_LEFT;
    keyBinds[static_cast<int>(ControlAction::UI_LEFT)].alternate = SDL_SCANCODE_A;
    keyBinds[static_cast<int>(ControlAction::UI_DOWN)].primary = SDL_SCANCODE_DOWN;
    keyBinds[static_cast<int>(ControlAction::UI_DOWN)].alternate = SDL_SCANCODE_S;
    keyBinds[static_cast<int>(ControlAction::UI_UP)].primary = SDL_SCANCODE_UP;
    keyBinds[static_cast<int>(ControlAction::UI_UP)].alternate = SDL_SCANCODE_W;
    keyBinds[static_cast<int>(ControlAction::UI_RIGHT)].primary = SDL_SCANCODE_RIGHT;
    keyBinds[static_cast<int>(ControlAction::UI_RIGHT)].alternate = SDL_SCANCODE_D;
    
    keyBinds[static_cast<int>(ControlAction::ACCEPT)].primary = SDL_SCANCODE_RETURN;
    keyBinds[static_cast<int>(ControlAction::ACCEPT)].alternate = SDL_SCANCODE_SPACE;
    keyBinds[static_cast<int>(ControlAction::BACK)].primary = SDL_SCANCODE_ESCAPE;
    keyBinds[static_cast<int>(ControlAction::BACK)].alternate = SDL_SCANCODE_BACKSPACE;
    keyBinds[static_cast<int>(ControlAction::PAUSE)].primary = SDL_SCANCODE_ESCAPE;
    keyBinds[static_cast<int>(ControlAction::PAUSE)].alternate = SDL_SCANCODE_RETURN;
    keyBinds[static_cast<int>(ControlAction::RESET)].primary = SDL_SCANCODE_R;
    keyBinds[static_cast<int>(ControlAction::RESET)].alternate = SDL_SCANCODE_UNKNOWN;
    
    gamepadBinds[static_cast<int>(ControlAction::NOTE_LEFT)].primary = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    gamepadBinds[static_cast<int>(ControlAction::NOTE_DOWN)].primary = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    gamepadBinds[static_cast<int>(ControlAction::NOTE_UP)].primary = SDL_CONTROLLER_BUTTON_DPAD_UP;
    gamepadBinds[static_cast<int>(ControlAction::NOTE_RIGHT)].primary = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    
    gamepadBinds[static_cast<int>(ControlAction::UI_LEFT)].primary = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    gamepadBinds[static_cast<int>(ControlAction::UI_DOWN)].primary = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    gamepadBinds[static_cast<int>(ControlAction::UI_UP)].primary = SDL_CONTROLLER_BUTTON_DPAD_UP;
    gamepadBinds[static_cast<int>(ControlAction::UI_RIGHT)].primary = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
    
    gamepadBinds[static_cast<int>(ControlAction::ACCEPT)].primary = SDL_CONTROLLER_BUTTON_A;
    gamepadBinds[static_cast<int>(ControlAction::BACK)].primary = SDL_CONTROLLER_BUTTON_B;
    gamepadBinds[static_cast<int>(ControlAction::PAUSE)].primary = SDL_CONTROLLER_BUTTON_START;
    gamepadBinds[static_cast<int>(ControlAction::RESET)].primary = SDL_CONTROLLER_BUTTON_Y;
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
        keyBinds[static_cast<int>(ControlAction::NOTE_LEFT)].primary = getScancodeFromString(mainBinds["left"].get<std::string>());
        keyBinds[static_cast<int>(ControlAction::NOTE_DOWN)].primary = getScancodeFromString(mainBinds["down"].get<std::string>());
        keyBinds[static_cast<int>(ControlAction::NOTE_UP)].primary = getScancodeFromString(mainBinds["up"].get<std::string>());
        keyBinds[static_cast<int>(ControlAction::NOTE_RIGHT)].primary = getScancodeFromString(mainBinds["right"].get<std::string>());
    }

    if (config.contains("altBinds")) {
        auto altBinds = config["altBinds"];
        keyBinds[static_cast<int>(ControlAction::NOTE_LEFT)].alternate = getScancodeFromString(altBinds["left"].get<std::string>());
        keyBinds[static_cast<int>(ControlAction::NOTE_DOWN)].alternate = getScancodeFromString(altBinds["down"].get<std::string>());
        keyBinds[static_cast<int>(ControlAction::NOTE_UP)].alternate = getScancodeFromString(altBinds["up"].get<std::string>());
        keyBinds[static_cast<int>(ControlAction::NOTE_RIGHT)].alternate = getScancodeFromString(altBinds["right"].get<std::string>());
    }

    if (config.contains("nxBinds")) {
        auto nxBinds = config["nxBinds"];
        gamepadBinds[static_cast<int>(ControlAction::NOTE_LEFT)].primary = getButtonFromString(nxBinds["left"].get<std::string>());
        gamepadBinds[static_cast<int>(ControlAction::NOTE_DOWN)].primary = getButtonFromString(nxBinds["down"].get<std::string>());
        gamepadBinds[static_cast<int>(ControlAction::NOTE_UP)].primary = getButtonFromString(nxBinds["up"].get<std::string>());
        gamepadBinds[static_cast<int>(ControlAction::NOTE_RIGHT)].primary = getButtonFromString(nxBinds["right"].get<std::string>());
    }

    if (config.contains("nxAltBinds")) {
        auto nxAltBinds = config["nxAltBinds"];
        gamepadBinds[static_cast<int>(ControlAction::NOTE_LEFT)].alternate = getButtonFromString(nxAltBinds["left"].get<std::string>());
        gamepadBinds[static_cast<int>(ControlAction::NOTE_DOWN)].alternate = getButtonFromString(nxAltBinds["down"].get<std::string>());
        gamepadBinds[static_cast<int>(ControlAction::NOTE_UP)].alternate = getButtonFromString(nxAltBinds["up"].get<std::string>());
        gamepadBinds[static_cast<int>(ControlAction::NOTE_RIGHT)].alternate = getButtonFromString(nxAltBinds["right"].get<std::string>());
    }

    if (config.contains("controlBinds")) {
        auto controlBinds = config["controlBinds"];
        
        for (int i = 0; i < static_cast<int>(ControlAction::COUNT); i++) {
            std::string actionName = getActionName(static_cast<ControlAction>(i));
            
            if (controlBinds.contains(actionName)) {
                auto actionData = controlBinds[actionName];
                
                if (actionData.contains("keyPrimary")) {
                    keyBinds[i].primary = getScancodeFromString(actionData["keyPrimary"].get<std::string>());
                }
                if (actionData.contains("keyAlternate")) {
                    keyBinds[i].alternate = getScancodeFromString(actionData["keyAlternate"].get<std::string>());
                }
                if (actionData.contains("padPrimary")) {
                    gamepadBinds[i].primary = getButtonFromString(actionData["padPrimary"].get<std::string>());
                }
                if (actionData.contains("padAlternate")) {
                    gamepadBinds[i].alternate = getButtonFromString(actionData["padAlternate"].get<std::string>());
                }
            }
        }
    }
}

void Controls::saveToConfig(const std::string& configPath) {
    nlohmann::json config;
    std::ifstream configFileIn(configPath);
    if (configFileIn.is_open()) {
        try {
            configFileIn >> config;
        } catch (const std::exception& ex) {
            std::cerr << "Failed to parse existing config.json: " << ex.what() << std::endl;
        }
        configFileIn.close();
    }

    nlohmann::json controlBinds = nlohmann::json::object();
    
    for (int i = 0; i < static_cast<int>(ControlAction::COUNT); i++) {
        std::string actionName = getActionName(static_cast<ControlAction>(i));
        nlohmann::json actionData = nlohmann::json::object();
        
        actionData["keyPrimary"] = getStringFromScancode(keyBinds[i].primary);
        actionData["keyAlternate"] = getStringFromScancode(keyBinds[i].alternate);
        actionData["padPrimary"] = getStringFromButton(gamepadBinds[i].primary);
        actionData["padAlternate"] = getStringFromButton(gamepadBinds[i].alternate);
        
        controlBinds[actionName] = actionData;
    }
    
    config["controlBinds"] = controlBinds;

    std::ofstream configFileOut(configPath);
    if (!configFileOut.is_open()) {
        std::cerr << "Failed to open config.json for writing" << std::endl;
        return;
    }
    
    configFileOut << config.dump(4) << std::endl;
    configFileOut.close();
}

bool Controls::justPressedAction(ControlAction action) const {
    int index = static_cast<int>(action);
    if (index < 0 || index >= static_cast<int>(ControlAction::COUNT)) return false;
    
    bool keyboardPressed = (keyBinds[index].primary != SDL_SCANCODE_UNKNOWN && 
                           flixel::FlxG::keys.keys[keyBinds[index].primary].justPressed()) || 
                          (keyBinds[index].alternate != SDL_SCANCODE_UNKNOWN && 
                           flixel::FlxG::keys.keys[keyBinds[index].alternate].justPressed());
    
    bool controllerPressed = (gamepadBinds[index].primary != SDL_CONTROLLER_BUTTON_INVALID && 
                              flixel::FlxG::gamepads.justPressed(gamepadBinds[index].primary)) ||
                            (gamepadBinds[index].alternate != SDL_CONTROLLER_BUTTON_INVALID && 
                             flixel::FlxG::gamepads.justPressed(gamepadBinds[index].alternate));
    
    return keyboardPressed || controllerPressed;
}

bool Controls::pressedAction(ControlAction action) const {
    int index = static_cast<int>(action);
    if (index < 0 || index >= static_cast<int>(ControlAction::COUNT)) return false;
    
    bool keyboardPressed = (keyBinds[index].primary != SDL_SCANCODE_UNKNOWN && 
                           flixel::FlxG::keys.keys[keyBinds[index].primary].pressed()) || 
                          (keyBinds[index].alternate != SDL_SCANCODE_UNKNOWN && 
                           flixel::FlxG::keys.keys[keyBinds[index].alternate].pressed());
    
    bool controllerPressed = (gamepadBinds[index].primary != SDL_CONTROLLER_BUTTON_INVALID && 
                              flixel::FlxG::gamepads.pressed(gamepadBinds[index].primary)) ||
                            (gamepadBinds[index].alternate != SDL_CONTROLLER_BUTTON_INVALID && 
                             flixel::FlxG::gamepads.pressed(gamepadBinds[index].alternate));
    
    return keyboardPressed || controllerPressed;
}

bool Controls::justReleasedAction(ControlAction action) const {
    int index = static_cast<int>(action);
    if (index < 0 || index >= static_cast<int>(ControlAction::COUNT)) return false;
    
    bool keyboardReleased = (keyBinds[index].primary != SDL_SCANCODE_UNKNOWN && 
                            flixel::FlxG::keys.keys[keyBinds[index].primary].justReleased()) || 
                           (keyBinds[index].alternate != SDL_SCANCODE_UNKNOWN && 
                            flixel::FlxG::keys.keys[keyBinds[index].alternate].justReleased());
    
    bool controllerReleased = (gamepadBinds[index].primary != SDL_CONTROLLER_BUTTON_INVALID && 
                               flixel::FlxG::gamepads.justReleased(gamepadBinds[index].primary)) ||
                             (gamepadBinds[index].alternate != SDL_CONTROLLER_BUTTON_INVALID && 
                              flixel::FlxG::gamepads.justReleased(gamepadBinds[index].alternate));
    
    return keyboardReleased || controllerReleased;
}

void Controls::rebindKey(ControlAction action, bool isPrimary, SDL_Scancode newKey) {
    int index = static_cast<int>(action);
    if (index >= 0 && index < static_cast<int>(ControlAction::COUNT)) {
        if (isPrimary) {
            keyBinds[index].primary = newKey;
        } else {
            keyBinds[index].alternate = newKey;
        }
    }
}

void Controls::rebindGamepad(ControlAction action, bool isPrimary, SDL_GameControllerButton newButton) {
    int index = static_cast<int>(action);
    if (index >= 0 && index < static_cast<int>(ControlAction::COUNT)) {
        if (isPrimary) {
            gamepadBinds[index].primary = newButton;
        } else {
            gamepadBinds[index].alternate = newButton;
        }
    }
}

Controls::KeyBind Controls::getKeyBindForAction(ControlAction action) const {
    int index = static_cast<int>(action);
    if (index >= 0 && index < static_cast<int>(ControlAction::COUNT)) {
        return keyBinds[index];
    }
    return {SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN};
}

Controls::GamepadBind Controls::getGamepadBindForAction(ControlAction action) const {
    int index = static_cast<int>(action);
    if (index >= 0 && index < static_cast<int>(ControlAction::COUNT)) {
        return gamepadBinds[index];
    }
    return {SDL_CONTROLLER_BUTTON_INVALID, SDL_CONTROLLER_BUTTON_INVALID};
}

std::string Controls::getActionName(ControlAction action) const {
    switch (action) {
        case ControlAction::NOTE_LEFT: return "NOTE_LEFT";
        case ControlAction::NOTE_DOWN: return "NOTE_DOWN";
        case ControlAction::NOTE_UP: return "NOTE_UP";
        case ControlAction::NOTE_RIGHT: return "NOTE_RIGHT";
        case ControlAction::UI_LEFT: return "UI_LEFT";
        case ControlAction::UI_DOWN: return "UI_DOWN";
        case ControlAction::UI_UP: return "UI_UP";
        case ControlAction::UI_RIGHT: return "UI_RIGHT";
        case ControlAction::ACCEPT: return "ACCEPT";
        case ControlAction::BACK: return "BACK";
        case ControlAction::PAUSE: return "PAUSE";
        case ControlAction::RESET: return "RESET";
        default: return "UNKNOWN";
    }
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
        {"LEFT", SDL_SCANCODE_LEFT}, {"RIGHT", SDL_SCANCODE_RIGHT},
        {"UP", SDL_SCANCODE_UP}, {"DOWN", SDL_SCANCODE_DOWN},
        {"SPACE", SDL_SCANCODE_SPACE}, {"ENTER", SDL_SCANCODE_RETURN},
        {"ESCAPE", SDL_SCANCODE_ESCAPE}, {"BACKSPACE", SDL_SCANCODE_BACKSPACE},
        {"TAB", SDL_SCANCODE_TAB}, {"LSHIFT", SDL_SCANCODE_LSHIFT},
        {"RSHIFT", SDL_SCANCODE_RSHIFT}, {"LCTRL", SDL_SCANCODE_LCTRL},
        {"RCTRL", SDL_SCANCODE_RCTRL}, {"LALT", SDL_SCANCODE_LALT},
        {"RALT", SDL_SCANCODE_RALT}, {"---", SDL_SCANCODE_UNKNOWN}
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
        {"LEFT", SDL_CONTROLLER_BUTTON_DPAD_LEFT},
        {"RIGHT", SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
        {"UP", SDL_CONTROLLER_BUTTON_DPAD_UP},
        {"DOWN", SDL_CONTROLLER_BUTTON_DPAD_DOWN},
        {"LSHOULDER", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"RSHOULDER", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"LSTICK", SDL_CONTROLLER_BUTTON_LEFTSTICK},
        {"RSTICK", SDL_CONTROLLER_BUTTON_RIGHTSTICK},
        {"START", SDL_CONTROLLER_BUTTON_START},
        {"BACK", SDL_CONTROLLER_BUTTON_BACK},
        {"---", SDL_CONTROLLER_BUTTON_INVALID}
    };

    auto it = buttonMap.find(buttonName);
    if (it != buttonMap.end()) {
        return it->second;
    }

    std::cout << "Unknown button name: " << buttonName << std::endl;
    return SDL_CONTROLLER_BUTTON_INVALID;
}

std::string Controls::getStringFromScancode(SDL_Scancode scancode) {
    static const std::map<SDL_Scancode, std::string> scancodeMap = {
        {SDL_SCANCODE_A, "A"}, {SDL_SCANCODE_B, "B"}, {SDL_SCANCODE_C, "C"},
        {SDL_SCANCODE_D, "D"}, {SDL_SCANCODE_E, "E"}, {SDL_SCANCODE_F, "F"},
        {SDL_SCANCODE_G, "G"}, {SDL_SCANCODE_H, "H"}, {SDL_SCANCODE_I, "I"},
        {SDL_SCANCODE_J, "J"}, {SDL_SCANCODE_K, "K"}, {SDL_SCANCODE_L, "L"},
        {SDL_SCANCODE_M, "M"}, {SDL_SCANCODE_N, "N"}, {SDL_SCANCODE_O, "O"},
        {SDL_SCANCODE_P, "P"}, {SDL_SCANCODE_Q, "Q"}, {SDL_SCANCODE_R, "R"},
        {SDL_SCANCODE_S, "S"}, {SDL_SCANCODE_T, "T"}, {SDL_SCANCODE_U, "U"},
        {SDL_SCANCODE_V, "V"}, {SDL_SCANCODE_W, "W"}, {SDL_SCANCODE_X, "X"},
        {SDL_SCANCODE_Y, "Y"}, {SDL_SCANCODE_Z, "Z"},
        {SDL_SCANCODE_LEFT, "LEFT"}, {SDL_SCANCODE_RIGHT, "RIGHT"},
        {SDL_SCANCODE_UP, "UP"}, {SDL_SCANCODE_DOWN, "DOWN"},
        {SDL_SCANCODE_SPACE, "SPACE"}, {SDL_SCANCODE_RETURN, "ENTER"},
        {SDL_SCANCODE_ESCAPE, "ESCAPE"}, {SDL_SCANCODE_BACKSPACE, "BACKSPACE"},
        {SDL_SCANCODE_TAB, "TAB"}, {SDL_SCANCODE_LSHIFT, "LSHIFT"},
        {SDL_SCANCODE_RSHIFT, "RSHIFT"}, {SDL_SCANCODE_LCTRL, "LCTRL"},
        {SDL_SCANCODE_RCTRL, "RCTRL"}, {SDL_SCANCODE_LALT, "LALT"},
        {SDL_SCANCODE_RALT, "RALT"}, {SDL_SCANCODE_UNKNOWN, "---"}
    };

    auto it = scancodeMap.find(scancode);
    if (it != scancodeMap.end()) {
        return it->second;
    }

    return "???";
}

std::string Controls::getStringFromButton(SDL_GameControllerButton button) {
    static const std::map<SDL_GameControllerButton, std::string> buttonMap = {
        {SDL_CONTROLLER_BUTTON_A, "A"}, {SDL_CONTROLLER_BUTTON_B, "B"},
        {SDL_CONTROLLER_BUTTON_X, "X"}, {SDL_CONTROLLER_BUTTON_Y, "Y"},
        {SDL_CONTROLLER_BUTTON_DPAD_LEFT, "LEFT"},
        {SDL_CONTROLLER_BUTTON_DPAD_RIGHT, "RIGHT"},
        {SDL_CONTROLLER_BUTTON_DPAD_UP, "UP"},
        {SDL_CONTROLLER_BUTTON_DPAD_DOWN, "DOWN"},
        {SDL_CONTROLLER_BUTTON_LEFTSHOULDER, "LSHOULDER"},
        {SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, "RSHOULDER"},
        {SDL_CONTROLLER_BUTTON_LEFTSTICK, "LSTICK"},
        {SDL_CONTROLLER_BUTTON_RIGHTSTICK, "RSTICK"},
        {SDL_CONTROLLER_BUTTON_START, "START"},
        {SDL_CONTROLLER_BUTTON_BACK, "BACK"},
        {SDL_CONTROLLER_BUTTON_INVALID, "---"}
    };

    auto it = buttonMap.find(button);
    if (it != buttonMap.end()) {
        return it->second;
    }

    return "???";
}