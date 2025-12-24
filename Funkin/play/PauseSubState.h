#pragma once

#include <flixel/FlxSubState.h>
#include <flixel/FlxSprite.h>
#include <flixel/text/FlxText.h>
#include <flixel/sound/FlxSound.h>
#include "../ui/components/Alphabet.h"
#include <vector>
#include <string>

class PauseSubState : public flixel::FlxSubState {
public:
    PauseSubState();
    ~PauseSubState() override;

    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;

private:
    void regenMenu();
    void changeSelection(int change = 0);
    
    std::vector<Alphabet*> grpMenuShit;
    std::vector<std::string> pauseOG;
    std::vector<std::string> difficultyChoices;
    std::vector<std::string> menuItems;
    int curSelected;
    
    flixel::FlxSprite* bg;
    flixel::FlxText* levelInfo;
    flixel::FlxText* levelDifficulty;
    flixel::FlxText* deathCounter;
    flixel::FlxText* practiceText;
    flixel::FlxSound* pauseMusic;
};
