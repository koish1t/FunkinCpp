#include "PauseHandler.h"
#include "../game/GameConfig.h"
#include "input/Controls.h"
#include <flixel/FlxG.h>
#include <SDL2/SDL.h>
#include <iostream>

PauseHandler::PauseHandler()
    : wasPaused(false)
{
}

void PauseHandler::update(float elapsed, flixel::FlxSound* inst, flixel::FlxSound* vocals,
                          float& songPosition, unsigned int& musicStartTicks,
                          flixel::FlxSubState*& subState,
                          std::function<void(flixel::FlxSubState*)> openSubStateFunc,
                          std::function<void()> closeSubStateFunc) {
    bool justClosed = (wasPaused && !subState);
    
    if (justClosed) {
        if (musicStartTicks > 0) {
            musicStartTicks = SDL_GetTicks() - static_cast<unsigned int>(songPosition);
        }
        
        if (inst && inst->paused) {
            inst->resume();
        }
        if (vocals && vocals->paused) {
            vocals->resume();
        }
    }
    
    wasPaused = (subState != nullptr);

    Controls* controls = GameConfig::getInstance()->controls;
    bool pausePressed = controls->justPressedAction(ControlAction::PAUSE);
    
    if (pausePressed && !subState && !justClosed) {
        if (inst) {
            inst->pause();
        }
        if (vocals) {
            vocals->pause();
        }
        
        if (musicStartTicks > 0) {
            musicStartTicks = SDL_GetTicks() - static_cast<unsigned int>(songPosition);
        }
        
        PauseSubState* pauseSubState = new PauseSubState();
        openSubStateFunc(pauseSubState);
    }
}