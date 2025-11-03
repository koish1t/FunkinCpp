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
    void update(float elapsed);
    void draw();
    
private:
    std::vector<flixel::FlxSprite*> letters;
    int baseX;
    int baseY;
};