#pragma once

#include <flixel/FlxState.h>
#include <flixel/sound/FlxSound.h>
#include "play/song/Conductor.h"

class FunkinState : public flixel::FlxState {
public:
    FunkinState();
    virtual ~FunkinState();

    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;

    void updateBeat();
    void updateCurStep();
    void stepHit();
    virtual void beatHit();

    static const std::string soundExt;
    int curStep = 0;
    int curBeat = 0;
    float lastBeat = 0.0f;
    float lastStep = 0.0f;
};
