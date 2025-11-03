#pragma once

#include <flixel/FlxSprite.h>
#include <string>
#include <functional>

class AtlasMenuItem : public flixel::FlxSprite {
public:
    std::string name;
    std::function<void()> callback;
    int ID;
    bool fireInstantly;
    bool centered;
    
    AtlasMenuItem(const std::string& itemName, const std::string& atlasPath, std::function<void()> cb);
    virtual ~AtlasMenuItem();
    
    void changeAnim(const std::string& animName);
    virtual void update(float elapsed) override;
};