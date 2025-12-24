#include "FunkinState.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include "play/song/Conductor.h"
#include <flixel/FlxG.h>
#include <flixel/FlxGame.h>

using namespace flixel::addons::transition;

const std::string FunkinState::soundExt = ".ogg";

FunkinState::FunkinState()
    : lastBeat(0.0f)
    , lastStep(0.0f)
    , curStep(0)
    , curBeat(0)
    , _transition(nullptr)
    , _transitionActive(false) {
}

FunkinState::~FunkinState() {
    if (_transition) {
        delete _transition;
        _transition = nullptr;
    }
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
    
    if (_transition && _transitionActive) {
        _transition->update(elapsed);
        if (_transition->isFinished()) {
            _transitionActive = false;
        }
    }

    FlxState::update(elapsed);
}

void FunkinState::draw() {
    FlxState::draw();
    
    if (_transition && _transitionActive) {
        _transition->draw();
    }
}

void FunkinState::destroy() {
    if (_transition) {
        delete _transition;
        _transition = nullptr;
    }
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

    if (Conductor::stepCrochet == 0) {
        static bool warned = false;
        if (!warned) {
            std::cout << "[WARNING] Conductor::stepCrochet is 0! Cannot calculate curStep." << std::endl;
            warned = true;
        }
        return;
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

void FunkinState::startTransitionIn(float duration, flixel::util::FlxColor color, flixel::FlxPoint direction) {
    if (_transition) {
        delete _transition;
    }
    
    TransitionData data(TransitionType::FADE, color, duration, direction);
    _transition = new FlxTransitionEffect(data);
    _transition->setStatus(TransitionStatus::FULL);
    _transition->start(TransitionStatus::OUT);
    _transitionActive = true;
}

void FunkinState::startTransitionOut(float duration, flixel::util::FlxColor color, flixel::FlxPoint direction, std::function<void()> onComplete) {
    if (_transition) {
        delete _transition;
    }
    
    TransitionData data(TransitionType::FADE, color, duration, direction);
    _transition = new FlxTransitionEffect(data);
    _transition->finishCallback = onComplete;
    _transition->setStatus(TransitionStatus::EMPTY);
    _transition->start(TransitionStatus::IN);
    _transitionActive = true;
}