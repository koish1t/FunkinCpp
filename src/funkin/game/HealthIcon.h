#pragma once

#include <flixel/FlxSprite.h>
#include <string>

class HealthIcon : public flixel::FlxSprite {
public:
    HealthIcon(const std::string& character = "bf", bool isPlayer = false);
    ~HealthIcon() override;
    
    void update(float elapsed) override;
    void changeIcon(const std::string& character);
    std::string getCharacter() const { return curCharacter; }
    
    bool isPlayer;
    flixel::FlxSprite* sprTracker;
    int fullIconWidth;
    
private:
    std::string curCharacter;
    float iconOffsets[2];
};