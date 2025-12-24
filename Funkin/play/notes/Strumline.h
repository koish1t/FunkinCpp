#pragma once

#include <vector>
#include <flixel/FlxSprite.h>
#include <flixel/FlxCamera.h>
#include <flixel/graphics/frames/FlxAtlasFrames.h>

class Strumline {
public:
    Strumline(float x, float y, int player, flixel::FlxCamera* camera);
    ~Strumline();
    
    void update(float elapsed);
    void draw();
    
    void playNote(int direction, bool confirm);
    void playStatic(int direction);
    void playPressed(int direction);
    void fadeInArrows();
    
    std::vector<flixel::FlxSprite*>& getNotes() { return notes; }
    flixel::FlxSprite* getNote(int index);
    
private:
    std::vector<flixel::FlxSprite*> notes;
    float x;
    float y;
    int player;
    flixel::FlxCamera* camera;
    
    void createNote(int i, float xPos, float yPos);
    void fadeInArrow(int index, flixel::FlxSprite* arrow);
};