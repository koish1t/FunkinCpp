#pragma once

#include <flixel/FlxSprite.h>
#include <flixel/FlxCamera.h>
#include <flixel/math/FlxPoint.h>
#include <vector>
#include <string>
#include <map>

class Digit : public flixel::FlxSprite {
public:
    std::map<std::string, float> offsetMap;
    
    Digit(float _x, float _y, const std::string& _path);
    void setOffset();
};

class DigitDisplay {
public:
    DigitDisplay(float _x, float _y, const std::string& _path, int _digitCount, float _scale = 1.0f, float _spacing = 0.0f, int _startingNumber = 0, bool _hideTrailingZeroes = false);
    ~DigitDisplay();
    
    void repositionDigits();
    void setNumber(int number, bool forceAllDigitsToAnimate = false);
    void tweenNumber(int newNumber, float tweenTime, bool force = false);
    void setDigitOffset(int number, float offset);
    void update(float elapsed);
    void draw();
    
    float x;
    float y;
    flixel::FlxPoint scrollFactor;
    flixel::FlxCamera* camera;
    bool isTweening;

private:
    float lastX;
    float lastY;
    std::string numString;
    bool hideTrailingZeroes;
    float digitScale;
    float spacing;
    int digitCount;
    std::vector<Digit*> digits;
    
    int targetNumber;
    int currentNumber;
    float tweenProgress;
    float tweenDuration;
};