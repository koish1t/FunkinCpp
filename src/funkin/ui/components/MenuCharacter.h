#pragma once

#include <flixel/FlxSprite.h>
#include <string>

class MenuCharacter : public flixel::FlxSprite {
public:
    std::string character;
    
    MenuCharacter(float x, const std::string& characterName);
    virtual ~MenuCharacter();

    void update(float elapsed) override;
};