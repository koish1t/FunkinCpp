#pragma once

#include <flixel/FlxSprite.h>
#include <flixel/FlxCamera.h>
#include "HealthIcon.h"

class HealthBar {
public:
    HealthBar(float x, float y, flixel::FlxCamera* camera);
    ~HealthBar();
    
    void setHealth(float health);
    float getHealth() const { return health; }
    
    void setIcons(const std::string& player, const std::string& opponent);
    void setColors(int player1R, int player1G, int player1B, int player2R, int player2G, int player2B);
    void update(float elapsed);
    void draw();
    
private:
    float health;
    flixel::FlxSprite* background;
    flixel::FlxSprite* redBar;
    flixel::FlxSprite* greenBar;
    HealthIcon* iconP1;
    HealthIcon* iconP2;
    flixel::FlxCamera* camera;
};