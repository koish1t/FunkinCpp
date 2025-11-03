#include "AtlasMenuItem.h"
#include <flixel/graphics/frames/FlxAtlasFrames.h>
#include <flixel/animation/FlxAnimationController.h>
#include <fstream>
#include <sstream>
#include <iostream>

AtlasMenuItem::AtlasMenuItem(const std::string& itemName, const std::string& atlasPath, std::function<void()> cb)
    : name(itemName)
    , callback(cb)
    , ID(0)
    , fireInstantly(false)
    , centered(false)
{
    std::string pngPath = atlasPath + ".png";
    std::string xmlPath = atlasPath + ".xml";
    
    std::ifstream file(xmlPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open atlas XML: " << xmlPath << std::endl;
        return;
    }
    
    std::string xmlText((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    auto atlasFrames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow(pngPath, xmlText);
    if (!atlasFrames) {
        std::cerr << "Failed to create atlas frames for: " << pngPath << std::endl;
        return;
    }
    
    frames = atlasFrames;
    texture = atlasFrames->texture;
    ownsTexture = false;
    animation = new flixel::animation::FlxAnimationController();
    
    std::string idlePrefix = itemName + " idle";
    std::string selectedPrefix = itemName + " selected";
    
    auto idleIndices = atlasFrames->getFramesByPrefix(idlePrefix);
    auto selectedIndices = atlasFrames->getFramesByPrefix(selectedPrefix);
    
    if (!idleIndices.empty()) {
        animation->addByPrefix("idle", idleIndices, 24, true);
    }
    if (!selectedIndices.empty()) {
        animation->addByPrefix("selected", selectedIndices, 24, true);
    }
    
    animation->play("idle");
    
    if (!atlasFrames->frames.empty()) {
        const auto& firstFrame = atlasFrames->frames[animation->getCurrentFrame()];
        frameWidth = firstFrame.sourceSize.w;
        frameHeight = firstFrame.sourceSize.h;
        width = static_cast<float>(frameWidth);
        height = static_cast<float>(frameHeight);
        updateHitbox();
    }
}

AtlasMenuItem::~AtlasMenuItem() {
}

void AtlasMenuItem::changeAnim(const std::string& animName) {
    if (animation) {
        animation->play(animName, true);
    }
}

void AtlasMenuItem::update(float elapsed) {
    FlxSprite::update(elapsed);
    
    if (frames && animation && !frames->frames.empty()) {
        int frameIdx = animation->getCurrentFrame();
        if (frameIdx >= 0 && frameIdx < static_cast<int>(frames->frames.size())) {
            const auto& frame = frames->frames[frameIdx];
            frameWidth = frame.sourceSize.w;
            frameHeight = frame.sourceSize.h;
            width = static_cast<float>(frameWidth);
            height = static_cast<float>(frameHeight);
        }
    }
}