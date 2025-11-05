#include "PauseSubState.h"
#include "../game/GameConfig.h"
#include "input/Controls.h"
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
    
    Controls* controls = GameConfig::getInstance()->controls;
    bool unpausePressed = controls->justPressedAction(ControlAction::PAUSE) ||
                         controls->justPressedAction(ControlAction::BACK);
    
    if (unpausePressed) {
        close();
    }
}

void PauseSubState::draw() {
    FlxSubState::draw();
}
