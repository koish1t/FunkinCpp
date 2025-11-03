#include "Alphabet.h"
#include <flixel/FlxG.h>
#include <flixel/graphics/frames/FlxAtlasFrames.h>
#include <cctype>
#include <fstream>
#include <sstream>

static flixel::graphics::frames::FlxAtlasFrames* alphabetFrames = nullptr;

Alphabet::Alphabet(const std::string& text, int x, int y) 
    : baseX(x)
    , baseY(y) 
{
    if (!alphabetFrames) {
        std::ifstream file("assets/images/alphabet.xml");
        std::string xmlText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        alphabetFrames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow(
            "assets/images/alphabet.png",
            xmlText
        );
    }
    
    int curX = x;
    int curY = y;
    const int letterSpacing = 48;
    const int spaceWidth = 48;
    const int lineHeight = 60;
    
    for (size_t i = 0; i < text.size(); ++i) {
        char c = text[i];
        
        if (c == '\n') {
            curX = x;
            curY += lineHeight;
            continue;
        }
        
        if (c == ' ') {
            letters.push_back(nullptr);
            curX += spaceWidth;
            continue;
        }
        
        flixel::FlxSprite* spr = new flixel::FlxSprite(curX, curY);
        spr->frames = alphabetFrames;
        spr->texture = alphabetFrames->texture;
        spr->ownsTexture = false;
        spr->animation = new flixel::animation::FlxAnimationController();
        
        std::string animName;
        if (std::isdigit(c)) {
            animName = std::string(1, c);
        } else if (std::isalpha(c)) {
            animName = std::string(1, c);
            if (std::isupper(c)) {
                animName += " bold";
            } else {
                animName += " lowercase";
            }
        } else {
            animName = std::string(1, c);
        }
        
        auto frameIndices = alphabetFrames->getFramesByPrefix(animName);
        if (!frameIndices.empty()) {
            spr->animation->addByPrefix(animName, frameIndices, 24, false);
            spr->animation->play(animName);
            spr->updateHitbox();
        }
        
        letters.push_back(spr);
        curX += letterSpacing;
    }
}

Alphabet::~Alphabet() {
    for (auto* spr : letters) {
        if (spr) {
            delete spr;
        }
    }
    letters.clear();
}

void Alphabet::screenCenter() {
    int totalWidth = static_cast<int>(letters.size()) * 48;
    int startX = (flixel::FlxG::width / 2) - (totalWidth / 2);
    int curX = startX;
    int y = baseY;
    
    for (auto* spr : letters) {
        if (spr) {
            spr->x = curX;
            spr->y = y;
            curX += 48;
        } else {
            curX += 48;
        }
    }
}

void Alphabet::setAlpha(float alpha) {
    for (auto* spr : letters) {
        if (spr) {
            spr->alpha = alpha;
        }
    }
}

void Alphabet::setVisible(bool visible) {
    for (auto* spr : letters) {
        if (spr) {
            spr->visible = visible;
        }
    }
}

void Alphabet::update(float elapsed) {
    for (auto* spr : letters) {
        if (spr) {
            spr->update(elapsed);
        }
    }
}

void Alphabet::draw() {
    for (auto* spr : letters) {
        if (spr && spr->visible) {
            spr->draw();
        }
    }
}