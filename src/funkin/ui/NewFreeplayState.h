#pragma once

#include "../FunkinState.h"
#include "freeplay/Capsule.h"
#include "freeplay/DigitDisplay.h"
#include "components/AtlasMenuItem.h"
#include <flixel/FlxSprite.h>
#include <flixel/FlxObject.h>
#include <flixel/FlxCamera.h>
#include <flixel/text/FlxText.h>
#include <flixel/math/FlxPoint.h>
#include <flixel/addons/FlxBackdrop.h>
#include <flixel/text/FlxBitmapText.h>
#include <vector>
#include <string>
#include <map>

class NewFreeplayState : public FunkinState {
public:
    NewFreeplayState(bool _transitionFromMenu = false, flixel::FlxPoint camFollowPos = flixel::FlxPoint(0.0f, 0.0f));
    virtual ~NewFreeplayState();
    
    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;
    void beatHit() override;
    
private:
    flixel::FlxSprite* bg;
    flixel::FlxSprite* flash;
    flixel::FlxSprite* fakeMenuBg;
    std::vector<AtlasMenuItem*> fakeMenuItems;
    flixel::FlxSprite* cover;
    flixel::FlxSprite* topBar;
    flixel::FlxText* freeplayText;
    flixel::FlxSprite* highscoreSprite;
    flixel::FlxSprite* clearPercentSprite;
    DigitDisplay* scoreDisplay;
    DigitDisplay* percentDisplay;
    flixel::FlxSprite* albumTitle;
    flixel::FlxSprite* arrowLeft;
    flixel::FlxSprite* arrowRight;
    flixel::FlxSprite* difficulty;
    flixel::FlxBitmapText* categoryTitle;
    flixel::FlxBitmapFont* categoryFont;
    flixel::FlxSprite* miniArrowLeft;
    flixel::FlxSprite* miniArrowRight;
    
    flixel::FlxSprite* album;
    flixel::FlxObject* albumDummy;
    float albumTime;
    std::string curAlbum;
    static constexpr float albumPeriod = 1.0f / 24.0f;
    
    std::vector<Capsule*> capsules;
    std::vector<std::string> categoryNames;
    std::map<std::string, std::vector<Capsule*>> categoryMap;
    
    flixel::FlxSprite* dj;
    
    std::vector<flixel::addons::FlxBackdrop*> scrollingTexts;
    std::vector<flixel::FlxText*> scrollingTextObjects;
    
    int curSelected;
    int curDifficulty;
    int curCategory;
    
    std::vector<int> allowedDifficulties;
    
    bool transitionOver;
    bool waitForFirstUpdateToStart;
    
    bool pendingSongStart;
    float songStartTimer;
    static constexpr float songStartDelay = 1.5f;
    
    bool exitingToMenu;
    float exitTimer;
    static constexpr float transitionTimeExit = 0.7f;
    static constexpr float staggerTimeExit = 0.07f;
    
    flixel::FlxObject* camFollow;
    flixel::FlxPoint camTarget;
    
    bool transitionFromMenu;
    Uint32 musicStartTicks;
    
    flixel::FlxCamera* camMenu;
    flixel::FlxCamera* camFreeplay;
    
    int lerpScore;
    int intendedScore;
    int prevScore;
    int prevAccuracy;
    
    static constexpr float transitionTime = 1.0f;
    static constexpr float staggerTime = 0.1f;
    static constexpr float randomVariation = 0.04f;
    
    static constexpr float randomVariationExit = 0.03f;
    
    void createFreeplayStuff();
    void fakeMainMenuSetup();
    void djIntroFinish();
    void startFreeplaySong();
    
    void addSong(const std::string& _song, const std::string& _displayName, const std::string& _icon, int _week, const std::string& _album = "vol1", const std::vector<std::string>& categories = {"All"});
    void addCapsules();
    void updateCapsulePosition(int index);
    
    void changeSelected(int change);
    void changeDifficulty(int change);
    void changeCategory(int change);
    
    void updateScore();
    void updateAccuracy();
    void updateAlbum(bool doTween = true);
    void calcAvailableDifficulties();
    
    std::string diffNumberToDiffName(int diff);
    std::string capitalizeFirstLetter(const std::string& str);
    
    void tweenCapsulesOnScreen(float _transitionTime, float _randomVariation, float _staggerTime, float _distance = 1000.0f);
    void tweenCapsulesOffScreen(float _transitionTime, float _randomVariation, float _staggerTime, float _distance = 1000.0f);
    void exitAnimation();
    
    void addScrollingText(const std::string& text, float yPos, float scrollSpeed, int size, Uint32 color);
    void setUpScrollingTextAccept();
    void refreshScrollingText();
    
    void startSong();
};