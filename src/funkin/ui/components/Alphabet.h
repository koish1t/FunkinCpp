#pragma once

#include <vector>
#include <string>
#include <flixel/FlxSprite.h>

class Alphabet {
public:
    Alphabet(const std::string& text, int x, int y);
    ~Alphabet();
    
    void screenCenter();
    void setAlpha(float alpha);
    void setVisible(bool visible);
    void setText(const std::string& newText);
    void setScale(float scaleX, float scaleY);
    void update(float elapsed);
    void draw();
    
    int getY() const { return baseY; }
    void setY(int y) { baseY = y; }
    
    bool isMenuItem;
    int targetY;
    float alpha;
    
    flixel::FlxCamera* camera;
    
private:
    std::vector<flixel::FlxSprite*> letters;
    int baseX;
    int baseY;
    std::string currentText;
    
    void createLetters(const std::string& text);
    void clearLetters();
};