#include "Character.h"
#include <flixel/graphics/frames/FlxAtlasFrames.h>
#include "Conductor.h"
#include <fstream>
#include <sstream>
#include <iostream>

Character::Character(float x, float y, const std::string& character, bool isPlayer)
    : FlxSprite(x, y)
    , curCharacter(character)
    , isPlayer(isPlayer)
    , debugMode(false)
    , holdTimer(0.0f)
    , danced(false)
{
    loadCharacter();
    dance();
    
    if (isPlayer) {
        flipX = !flipX;
    }
}

Character::~Character() {
    animOffsets.clear();
}

void Character::loadCharacter() {
    if (curCharacter == "bf") {
        setupBF();
    } else if (curCharacter == "gf") {
        setupGF();
    } else if (curCharacter == "dad") {
        setupDad();
    }
}

void Character::setupBF() {
    std::ifstream file("assets/images/BOYFRIEND.xml");
    if (!file.is_open()) {
        std::cerr << "Failed to load BOYFRIEND.xml" << std::endl;
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xmlText = buffer.str();
    file.close();
    
    auto tex = flixel::graphics::frames::FlxAtlasFrames::fromSparrow(
        "assets/images/BOYFRIEND.png",
        xmlText
    );
    
    frames = tex;
    
    if (frames && !frames->frames.empty()) {
        const auto& firstFrame = frames->frames[0];
        frameWidth = firstFrame.sourceSize.w;
        frameHeight = firstFrame.sourceSize.h;
        width = static_cast<float>(frameWidth);
        height = static_cast<float>(frameHeight);
    }
    texture = tex->texture;
    ownsTexture = false;
    animation = new flixel::animation::FlxAnimationController();
    
    auto idleFrames = frames->getFramesByPrefix("BF idle dance");
    auto singUPFrames = frames->getFramesByPrefix("BF NOTE UP");
    auto singLEFTFrames = frames->getFramesByPrefix("BF NOTE LEFT");
    auto singRIGHTFrames = frames->getFramesByPrefix("BF NOTE RIGHT");
    auto singDOWNFrames = frames->getFramesByPrefix("BF NOTE DOWN");
    auto singUPmissFrames = frames->getFramesByPrefix("BF NOTE UP MISS");
    auto singLEFTmissFrames = frames->getFramesByPrefix("BF NOTE LEFT MISS");
    auto singRIGHTmissFrames = frames->getFramesByPrefix("BF NOTE RIGHT MISS");
    auto singDOWNmissFrames = frames->getFramesByPrefix("BF NOTE DOWN MISS");
    auto heyFrames = frames->getFramesByPrefix("BF HEY");
    
    if (!idleFrames.empty()) animation->addByPrefix("idle", idleFrames, 24, false);
    if (!singUPFrames.empty()) animation->addByPrefix("singUP", singUPFrames, 24, false);
    if (!singLEFTFrames.empty()) animation->addByPrefix("singLEFT", singLEFTFrames, 24, false);
    if (!singRIGHTFrames.empty()) animation->addByPrefix("singRIGHT", singRIGHTFrames, 24, false);
    if (!singDOWNFrames.empty()) animation->addByPrefix("singDOWN", singDOWNFrames, 24, false);
    if (!singUPmissFrames.empty()) animation->addByPrefix("singUPmiss", singUPmissFrames, 24, false);
    if (!singLEFTmissFrames.empty()) animation->addByPrefix("singLEFTmiss", singLEFTmissFrames, 24, false);
    if (!singRIGHTmissFrames.empty()) animation->addByPrefix("singRIGHTmiss", singRIGHTmissFrames, 24, false);
    if (!singDOWNmissFrames.empty()) animation->addByPrefix("singDOWNmiss", singDOWNmissFrames, 24, false);
    if (!heyFrames.empty()) animation->addByPrefix("hey", heyFrames, 24, false);
    
    addOffset("idle", -5, 0);
    addOffset("singUP", -29, 27);
    addOffset("singRIGHT", -38, -7);
    addOffset("singLEFT", 12, -6);
    addOffset("singDOWN", -10, -50);
    addOffset("singUPmiss", -29, 27);
    addOffset("singRIGHTmiss", -30, 21);
    addOffset("singLEFTmiss", 12, 24);
    addOffset("singDOWNmiss", -11, -19);
    addOffset("hey", 7, 4);
    
    playAnim("idle");
    flipX = true;
}

void Character::setupGF() {
    std::ifstream file("assets/images/GF_assets.xml");
    if (!file.is_open()) {
        std::cerr << "Failed to load GF_assets.xml" << std::endl;
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xmlText = buffer.str();
    file.close();
    
    auto tex = flixel::graphics::frames::FlxAtlasFrames::fromSparrow(
        "assets/images/GF_assets.png",
        xmlText
    );
    
    frames = tex;
    
    if (frames && !frames->frames.empty()) {
        const auto& firstFrame = frames->frames[0];
        frameWidth = firstFrame.sourceSize.w;
        frameHeight = firstFrame.sourceSize.h;
        width = static_cast<float>(frameWidth);
        height = static_cast<float>(frameHeight);
    }
    texture = tex->texture;
    ownsTexture = false;
    animation = new flixel::animation::FlxAnimationController();
    
    auto cheerFrames = frames->getFramesByPrefix("GF Cheer");
    auto singLEFTFrames = frames->getFramesByPrefix("GF left note");
    auto singRIGHTFrames = frames->getFramesByPrefix("GF Right Note");
    auto singUPFrames = frames->getFramesByPrefix("GF Up Note");
    auto singDOWNFrames = frames->getFramesByPrefix("GF Down Note");
    
    if (!cheerFrames.empty()) animation->addByPrefix("cheer", cheerFrames, 24, false);
    if (!singLEFTFrames.empty()) animation->addByPrefix("singLEFT", singLEFTFrames, 24, false);
    if (!singRIGHTFrames.empty()) animation->addByPrefix("singRIGHT", singRIGHTFrames, 24, false);
    if (!singUPFrames.empty()) animation->addByPrefix("singUP", singUPFrames, 24, false);
    if (!singDOWNFrames.empty()) animation->addByPrefix("singDOWN", singDOWNFrames, 24, false);
    
    std::vector<int> danceLeftIndices = {30, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    std::vector<int> danceRightIndices = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
    
    auto danceFrames = frames->getFramesByPrefix("GF Dancing Beat");
    if (!danceFrames.empty()) {
        animation->addByIndices("danceLeft", danceFrames, danceLeftIndices, 24, false);
        animation->addByIndices("danceRight", danceFrames, danceRightIndices, 24, false);
    }
    
    addOffset("cheer", 0, 0);
    addOffset("danceLeft", 0, -9);
    addOffset("danceRight", 0, -9);
    addOffset("singUP", 0, 4);
    addOffset("singRIGHT", 0, -20);
    addOffset("singLEFT", 0, -19);
    addOffset("singDOWN", 0, -20);
    
    danced = true;
    playAnim("danceRight");
}

void Character::setupDad() {
    std::ifstream file("assets/images/DADDY_DEAREST.xml");
    if (!file.is_open()) {
        std::cerr << "Failed to load DADDY_DEAREST.xml" << std::endl;
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xmlText = buffer.str();
    file.close();
    
    auto tex = flixel::graphics::frames::FlxAtlasFrames::fromSparrow(
        "assets/images/DADDY_DEAREST.png",
        xmlText
    );
    
    frames = tex;
    
    if (frames && !frames->frames.empty()) {
        const auto& firstFrame = frames->frames[0];
        frameWidth = firstFrame.sourceSize.w;
        frameHeight = firstFrame.sourceSize.h;
        width = static_cast<float>(frameWidth);
        height = static_cast<float>(frameHeight);
    }
    texture = tex->texture;
    ownsTexture = false;
    animation = new flixel::animation::FlxAnimationController();
    
    auto idleFrames = frames->getFramesByPrefix("Dad idle dance");
    auto singUPFrames = frames->getFramesByPrefix("Dad Sing Note UP");
    auto singRIGHTFrames = frames->getFramesByPrefix("Dad Sing Note RIGHT");
    auto singDOWNFrames = frames->getFramesByPrefix("Dad Sing Note DOWN");
    auto singLEFTFrames = frames->getFramesByPrefix("Dad Sing Note LEFT");
    
    if (!idleFrames.empty()) animation->addByPrefix("idle", idleFrames, 24, true);
    if (!singUPFrames.empty()) animation->addByPrefix("singUP", singUPFrames, 24, false);
    if (!singRIGHTFrames.empty()) animation->addByPrefix("singRIGHT", singRIGHTFrames, 24, false);
    if (!singDOWNFrames.empty()) animation->addByPrefix("singDOWN", singDOWNFrames, 24, false);
    if (!singLEFTFrames.empty()) animation->addByPrefix("singLEFT", singLEFTFrames, 24, false);
    
    addOffset("idle", 0, 0);
    addOffset("singUP", -6, 50);
    addOffset("singRIGHT", 0, 27);
    addOffset("singLEFT", -10, 10);
    addOffset("singDOWN", 0, -30);
    
    playAnim("idle");
}

void Character::update(float elapsed) {
    FlxSprite::update(elapsed);
    
    if (animation) {
        animation->update(elapsed);
        
        if (frames && !frames->frames.empty()) {
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
    
    if (curCharacter.rfind("bf", 0) == 0) {
        holdTimer += elapsed;
        
        if (holdTimer >= Conductor::stepCrochet * 4 * 0.001f && animation) {
            if (animation->current.rfind("sing", 0) == 0 && animation->current.find("miss") == std::string::npos) {
                dance();
            }
        }
    } else {
        if (animation && animation->current.rfind("sing", 0) == 0) {
            holdTimer += elapsed;
            
            float dadVar = 4.0f;
            if (curCharacter == "dad") {
                dadVar = 6.1f;
            }
            
            if (holdTimer >= Conductor::stepCrochet * dadVar * 0.001f) {
                dance();
                holdTimer = 0.0f;
            }
        }
    }
}

void Character::dance() {
    if (!debugMode) {
        if (curCharacter == "gf") {
            danced = !danced;
            if (danced) {
                playAnim("danceRight", true);
            } else {
                playAnim("danceLeft", true);
            }
        } else {
            playAnim("idle", true);
        }
    }
}

void Character::playAnim(const std::string& animName, bool force, bool reversed, int frame) {
    if (animation) {
        animation->play(animName, force);
        
        if (animOffsets.find(animName) != animOffsets.end()) {
            auto& offset = animOffsets[animName];
            offsetX = offset[0];
            offsetY = offset[1];
        } else {
            offsetX = 0;
            offsetY = 0;
        }
        
        if (curCharacter == "gf") {
            if (animName == "singLEFT") {
                danced = true;
            } else if (animName == "singRIGHT") {
                danced = false;
            }
            
            if (animName == "singUP" || animName == "singDOWN") {
                danced = !danced;
            }
        }
    }
}

void Character::addOffset(const std::string& name, float x, float y) {
    animOffsets[name] = {x, y};
}