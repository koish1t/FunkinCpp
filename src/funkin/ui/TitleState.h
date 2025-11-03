#pragma once

#include "../FunkinState.h"
#include "components/Alphabet.h"
#include <flixel/FlxSprite.h>
#include <flixel/sound/FlxSound.h>
#include <vector>
#include <string>

class TitleState : public FunkinState {
public:
    TitleState();
    ~TitleState();
    
    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;
    void beatHit() override;
    
private:
    void createCoolText(const std::vector<std::string>& lines);
    void createMoreCoolText(const std::string& line);
    void removeText();
    void skipIntro();
    
    flixel::FlxSprite* logo;
    flixel::FlxSprite* gf;
    flixel::FlxSprite* enter;
    flixel::FlxSound* music;
    flixel::FlxSound* confirmSound;
    
    float whiteAlpha;
    bool skippedIntro;
    bool transitioning;
    Uint32 musicStartTicks;
    bool musicStarted;
    
    std::vector<Alphabet*> alphabets;
};