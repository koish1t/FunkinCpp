#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "PlayState.h"
#include <flixel/FlxG.h>
#include <flixel/graphics/frames/FlxAtlasFrames.h>
#include <flixel/animation/FlxAnimationController.h>

void PlayState::create() {
    std::cout << "PlayState: Create Function Called!" << std::endl;
}

void PlayState::update(float elapsed) {
    auto* currentSubState = subState;
    
    FlxState::update(elapsed);
    
    if (currentSubState && !subState) {
        return;
    }
    
    if (!subState) {}
}

void PlayState::draw() {
    FlxState::draw();
}

void PlayState::destroy() {
    FlxState::destroy();
} 