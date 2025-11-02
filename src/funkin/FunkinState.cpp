#include "FunkinState.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include "game/Conductor.h"

const std::string FunkinState::soundExt = ".ogg";

FunkinState::FunkinState()
    : lastBeat(0.0f)
    , lastStep(0.0f)
    , curStep(0)
    , curBeat(0) {
}

FunkinState::~FunkinState() {
    destroy();
}

void FunkinState::create() {
    std::cout << "FunkinState::create()" << std::endl;
}

void FunkinState::update(float elapsed) {
    int oldStep = curStep;

    updateCurStep();
    updateBeat();

    if (oldStep != curStep && curStep > 0) {
        stepHit();
    }

    FlxState::update(elapsed);
}

void FunkinState::draw() {
    FlxState::draw();
}

void FunkinState::destroy() {
    FlxState::destroy();
}

void FunkinState::updateBeat() {
    curBeat = static_cast<int>(std::floor(curStep / 4.0f));
}

void FunkinState::updateCurStep() {
    BPMChangeEvent lastChange = {
        0,      // stepTime
        0.0f,   // songTime
        0       // bpm
    };

    for (const auto& change : Conductor::bpmChangeMap) {
        if (Conductor::songPosition >= change.songTime) {
            lastChange = change;
        }
    }

    float stepsSinceChange = (Conductor::songPosition - lastChange.songTime) / Conductor::stepCrochet;
    curStep = lastChange.stepTime + static_cast<int>(std::floor(stepsSinceChange));
}

void FunkinState::stepHit() {
    if (curStep % 4 == 0) {
        beatHit();
    }
}

void FunkinState::beatHit() {
    // become GAY!
}
