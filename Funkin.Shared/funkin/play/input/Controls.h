#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <map>
#include <vector>

enum class ControlAction {
    NOTE_LEFT = 0,
    NOTE_DOWN = 1,
    NOTE_UP = 2,
    NOTE_RIGHT = 3,
    UI_LEFT,
    UI_DOWN,
    UI_UP,
    UI_RIGHT,
    ACCEPT,
    BACK,
    PAUSE,
    RESET,
    COUNT
};

class Controls {
public:
    struct KeyBind {
        SDL_Scancode primary;
        SDL_Scancode alternate;
    };
    
    struct GamepadBind {
        SDL_GameControllerButton primary;
        SDL_GameControllerButton alternate;
    };
    
    Controls();
    
    void loadFromConfig(const std::string& configPath);
    void saveToConfig(const std::string& configPath);
    
    bool justPressedAction(ControlAction action) const;
    bool pressedAction(ControlAction action) const;
    bool justReleasedAction(ControlAction action) const;
    
    void rebindKey(ControlAction action, bool isPrimary, SDL_Scancode newKey);
    void rebindGamepad(ControlAction action, bool isPrimary, SDL_GameControllerButton newButton);
    
    KeyBind getKeyBindForAction(ControlAction action) const;
    GamepadBind getGamepadBindForAction(ControlAction action) const;
    
    std::string getActionName(ControlAction action) const;
    static SDL_Scancode getScancodeFromString(const std::string& keyName);
    static std::string getStringFromScancode(SDL_Scancode scancode);
    static SDL_GameControllerButton getButtonFromString(const std::string& buttonName);
    static std::string getStringFromButton(SDL_GameControllerButton button);
    
private:
    KeyBind keyBinds[static_cast<int>(ControlAction::COUNT)];
    GamepadBind gamepadBinds[static_cast<int>(ControlAction::COUNT)];
    
    void setDefaultBindings();
};