#include "PauseSubState.h"
#include <flixel/FlxG.h>

PauseSubState::PauseSubState() {
    pauseText = nullptr;
}

void PauseSubState::create() {
    pauseText = new flixel::FlxText(0, 0, 0, "PAUSED");
    pauseText->setFont("assets/fonts/monsterrat.ttf");
    pauseText->setSize(64);
    pauseText->setPosition(flixel::FlxG::width / 2 - pauseText->width / 2, flixel::FlxG::height / 2 - pauseText->height / 2);
    add(pauseText);
}

void PauseSubState::update(float elapsed) {
    FlxSubState::update(elapsed);
    
    if (flixel::FlxG::keys.keys[SDL_SCANCODE_RETURN].justPressed() || flixel::FlxG::keys.keys[SDL_SCANCODE_ESCAPE].justPressed()) {
        close();
    }
}

void PauseSubState::draw() {
    FlxSubState::draw();
}
