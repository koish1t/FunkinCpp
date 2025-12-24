#pragma once

#include <flixel/FlxSprite.h>
#include <string>

class MenuItem : public flixel::FlxSprite {
public:
    float targetY;
    
    MenuItem(float x, float y, const std::string& weekFileName);
    virtual ~MenuItem();
    
    void startFlashing();
    virtual void update(float elapsed) override;
};