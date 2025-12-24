#pragma once

#include <flixel/FlxSprite.h>
#include <flixel/graphics/frames/FlxAtlasFrames.h>
#include <flixel/animation/FlxAnimationController.h>
#include <map>
#include <string>

class NoteSprite : public flixel::FlxSprite {
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

    NoteSprite(float strumTime, int noteData);
    ~NoteSprite();

    void update(float elapsed) override;
    void draw() override;

    float strumTime;
    int noteData;
    float sustainLength = 0.0f;
    bool mustPress = false;
    bool canBeHit = false;
    bool wasGoodHit = false;
    bool tooLate = false;
    float noteScore = 1.0f;
    bool kill = false;    
    float hitTime = 0.0f;
    bool isHolding = false;
    bool holdReleased = false;
    float holdClipTime = 0.0f;

private:
    int headFrameIndex = -1;
    int bodyFrameIndex = -1;
    int tailFrameIndex = -1;

    void cacheFrameIndices();
    void drawFrame(int frameIndex, float drawX, float drawY, float scaleX, float scaleY);
    float getHoldClipAmount() const;
};
