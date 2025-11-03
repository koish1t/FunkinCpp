#pragma once

#include <flixel/FlxSprite.h>
#include <flixel/sound/FlxSound.h>
#include <string>

class Countdown {
public:
    Countdown();
    ~Countdown();
    
    void start(float crochet);
    void update(float elapsed);
    bool isFinished() const;
    
    flixel::FlxSprite* getCurrentSprite();
    
private:
    int step;
    float timer;
    float interval;
    float spriteTimer;
    float spriteStartY;
    bool finished;
    
    flixel::FlxSound* sound;
    flixel::FlxSprite* sprite;
    
    std::string soundExt;
    
    void nextStep();
    void createSprite(const std::string& imagePath);
};