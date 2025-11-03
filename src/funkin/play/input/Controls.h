#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <map>

class Controls {
public:
    struct KeyBind {
        SDL_Scancode primary;
        SDL_Scancode alternate;
    };
    
    struct NXKeyBind {
        SDL_GameControllerButton primary;
        SDL_GameControllerButton alternate;
    };
    
    Controls();
    
    void loadFromConfig(const std::string& configPath);
    
    bool justPressed(int direction) const;
    bool pressed(int direction) const;
    bool justReleased(int direction) const;
    
    KeyBind getKeyBind(int direction) const;
    NXKeyBind getNXKeyBind(int direction) const;
    
private:
    KeyBind arrowKeys[4];
    NXKeyBind nxArrowKeys[4];
    
    SDL_Scancode getScancodeFromString(const std::string& keyName);
    SDL_GameControllerButton getButtonFromString(const std::string& buttonName);
};