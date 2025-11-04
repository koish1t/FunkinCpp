#pragma once

#include <flixel/FlxSprite.h>
#include <flixel/text/FlxText.h>
#include <flixel/math/FlxPoint.h>
#include <string>

class Capsule : public flixel::FlxSprite {
public:
    std::string song;
    std::string album;
    int week;
    
    flixel::FlxPoint targetPos;
    float xPositionOffset;
    bool selected;
    
    flixel::FlxSprite* capsuleSprite;
    flixel::FlxSprite* icon;
    flixel::FlxText* text;
    
    Capsule(const std::string& _song, const std::string& _displayName, const std::string& _icon, int _week, const std::string& _album = "vol1");
    virtual ~Capsule();
    
    void confirm();
    virtual void update(float elapsed) override;
    void select();
    void deselect();
    void snapToTargetPos();
    float intendedX(int index);
    float intendedY(int index);
    
private:
    static constexpr float capsuleScale = 0.8f;
    static constexpr float capsuleHeight = 132.0f;
    static constexpr uint32_t selectColor = 0xFFFFFFFF;
    static constexpr uint32_t deselectColor = 0xFF969A9D;
    static constexpr uint32_t selectBorderColor   = 0x6B9FBAFF;
    static constexpr uint32_t deselectBorderColor = 0x3E508CFF;
};