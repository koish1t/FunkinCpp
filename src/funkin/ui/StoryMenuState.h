#pragma once

#include "../FunkinState.h"
#include "components/MenuItem.h"
#include "components/MenuCharacter.h"
#include "components/Alphabet.h"
#include <flixel/text/FlxText.h>
#include <vector>
#include <string>

struct WeekData {
    std::vector<std::string> songs;
    std::vector<std::string> weekCharacters;
    std::string weekName;
    std::string weekFileName;
    std::vector<int> difficulties;
    bool unlocked;
};

class StoryMenuState : public FunkinState {
public:
    StoryMenuState();
    virtual ~StoryMenuState();
    
    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;
    
private:
    flixel::FlxSprite* bg;
    flixel::FlxSprite* yellowBG;
    flixel::FlxSprite* blackBar;
    
    flixel::FlxText* scoreText;
    flixel::FlxText* txtWeekTitle;
    flixel::FlxText* txtTracklist;
    
    std::vector<MenuItem*> grpWeekText;
    std::vector<MenuCharacter*> grpWeekCharacters;
    std::vector<flixel::FlxSprite*> grpLocks;
    
    flixel::FlxSprite* leftArrow;
    flixel::FlxSprite* sprDifficulty;
    flixel::FlxSprite* rightArrow;
    
    std::vector<WeekData> weekData;
    int curWeek;
    int curDifficulty;
    
    float lerpScore;
    int intendedScore;
    
    bool movedBack;
    bool selectedWeek;
    bool stopspamming;
    
    std::vector<int> availableDifficulties;
    std::vector<std::string> difficultyNames;
    
    void loadWeeks();
    void changeWeek(int change);
    void changeDifficulty(int change = 0);
    void selectWeek();
    void updateText();
    void updateAvailableDifficulties();
};