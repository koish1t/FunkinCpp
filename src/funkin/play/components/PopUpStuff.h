#pragma once

#include <flixel/FlxSprite.h>
#include <flixel/FlxCamera.h>
#include <vector>
#include <string>

class PopUpStuff {
public:
    PopUpStuff();
    ~PopUpStuff();
    
    void popUpScore(const std::string& rating, int combo, float crochet, flixel::FlxCamera* camera);
    void update(float elapsed, float crochet);
    void draw();
    void clear();
    
private:
    std::vector<flixel::FlxSprite*> sprites;
    std::vector<float> timers;
};