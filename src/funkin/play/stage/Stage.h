#pragma once

#include <flixel/FlxSprite.h>
#include <string>
#include <vector>

class Stage {
public:
    Stage(const std::string& stageName);
    ~Stage();
    
    void addSpritesToState();
    
    float getDefaultZoom() const { return defaultZoom; }
    std::string getStageName() const { return curStage; }
    bool isPixelStage() const;
    
    std::vector<flixel::FlxSprite*>& getSprites() { return sprites; }
    
    static std::string getStageFromSong(const std::string& songName);
    
private:
    std::string curStage;
    float defaultZoom;
    std::vector<flixel::FlxSprite*> sprites;
    
    void buildStage();
    bool loadFromJSON(const std::string& stageName);
    void buildDefaultStage();
    
    flixel::FlxSprite* createSprite(float x, float y, const std::string& imagePath);
    flixel::FlxSprite* createAnimatedSprite(float x, float y, const std::string& imagePath, const std::string& xmlPath);
};