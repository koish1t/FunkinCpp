#include "PauseHandler.h"
#include "../game/GameConfig.h"
#include "input/Controls.h"
#include <flixel/FlxG.h>
#include <SDL2/SDL.h>
#include <iostream>

PauseHandler::PauseHandler()
    : pauseCooldown(0.0f)
    , wasPaused(false)
{
}

void PauseHandler::update(float elapsed, flixel::FlxSound* inst, flixel::FlxSound* vocals,
                          float& songPosition, unsigned int& musicStartTicks,
                          flixel::FlxSubState*& subState,
                          std::function<void(flixel::FlxSubState*)> openSubStateFunc,
                          std::function<void()> closeSubStateFunc) {
    if (wasPaused && !subState) {
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
    
    if (pauseCooldown > 0) {
        pauseCooldown -= elapsed;
    }

    Controls* controls = GameConfig::getInstance()->controls;
    bool pausePressed = controls->justPressedAction(ControlAction::PAUSE);
    
    if (pausePressed) {
        if (!subState) {
            if (pauseCooldown <= 0) {
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
                std::cout << "Pause SubState opened" << std::endl;
                pauseCooldown = 0.5f;
            }
        } else {
            if (musicStartTicks > 0) {
                musicStartTicks = SDL_GetTicks() - static_cast<unsigned int>(songPosition);
            }
            
            if (inst) {
                inst->resume();
            }
            if (vocals) {
                vocals->resume();
            }
            
            closeSubStateFunc();
            std::cout << "Pause SubState closed" << std::endl;
        }
    }
}