#pragma once

#include <flixel/FlxSprite.h>
#include <flixel/graphics/frames/FlxAtlasFrames.h>
#include <flixel/animation/FlxAnimationController.h>
#include <map>
#include <string>

class Note : public flixel::FlxSprite {
public:
    static constexpr int LEFT_NOTE = 0;
    static constexpr int DOWN_NOTE = 1;
    static constexpr int UP_NOTE = 2;
    static constexpr int RIGHT_NOTE = 3;

    static const float STRUM_X;
    static const float swagWidth;

    static bool assetsLoaded;
    static flixel::graphics::frames::FlxAtlasFrames* noteFrames;
    static void loadAssets();
    static void unloadAssets();
    static float getTargetY();

    Note(float strumTime, int noteData, Note* prevNote = nullptr, bool sustainNote = false);
    ~Note();

    void update(float elapsed) override;
    void setupNote();
    void setupSustainNote();

    float strumTime;
    int noteData;
    float sustainLength;
    bool mustPress;
    bool isSustainNote;
    bool canBeHit;
    bool wasGoodHit;
    bool tooLate;
    float noteScore;
    Note* prevNote;
    bool kill = false;
};
