#pragma once

#include <flixel/FlxSubState.h>
#include <flixel/text/FlxText.h>

class PauseSubState : public flixel::FlxSubState {
public:
    PauseSubState();
    ~PauseSubState() override = default;

    void create() override;
    void update(float elapsed) override;
    void draw() override;

private:
    flixel::FlxText* pauseText;
};
