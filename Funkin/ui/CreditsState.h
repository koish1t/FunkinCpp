#pragma once

#include "../FunkinState.h"
#include <flixel/FlxSprite.h>
#include <flixel/text/FlxText.h>
#include <vector>
#include <string>

struct CreditsEntry {
    std::string header;
    std::vector<std::string> body;
};

class CreditsState : public FunkinState {
public:
    CreditsState();
    ~CreditsState();
    
    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;
    
private:
    void buildCreditsGroup();
    flixel::FlxText* buildCreditsLine(const std::string& text, float yPos, bool header);
    bool hasEnded();
    void exitCredits();
    
    static constexpr float SCREEN_PAD = 24.0f;
    static constexpr int CREDITS_FONT_SIZE = 24;
    static constexpr int CREDITS_HEADER_FONT_SIZE = 32;
    static constexpr float CREDITS_SCROLL_BASE_SPEED = 100.0f;
    static constexpr float CREDITS_SCROLL_FAST_SPEED = 400.0f;
    
    std::vector<CreditsEntry> creditsData;    
    flixel::FlxSprite* bg;
    std::vector<flixel::FlxText*> creditsLines;
    float creditsGroupY;
    float creditsGroupHeight;
    bool scrollPaused;
    float STARTING_HEIGHT;
    float FULL_WIDTH;
};