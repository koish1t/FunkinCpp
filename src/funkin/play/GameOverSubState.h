#pragma once

#include <flixel/FlxSubState.h>
#include <flixel/FlxObject.h>
#include <flixel/FlxSprite.h>
#include <flixel/FlxCamera.h>
#include <flixel/sound/FlxSound.h>
#include "character/Character.h"

class GameOverSubState : public flixel::FlxSubState {
public:
    GameOverSubState(float x, float y, flixel::FlxCamera* camera);
    ~GameOverSubState() override;
    
    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;
    void beatHit();
    
private:
    void endBullshit();
    
    flixel::FlxSprite* blackScreen;
    Character* bf;
    flixel::FlxObject* camFollow;
    flixel::FlxSound* gameOverMusic;
    flixel::FlxCamera* gameCamera;
    
    bool isEnding;
    float bfX;
    float bfY;
};