#include "Note.h"
#include "../states/PlayState.h"
#include "Conductor.h"
#include "GameConfig.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

const float Note::STRUM_X = 42.0f;
const float Note::swagWidth = 160.0f * 0.7f;
bool Note::assetsLoaded = false;
flixel::graphics::frames::FlxAtlasFrames* Note::noteFrames = nullptr;

void Note::loadAssets() {
    if (!assetsLoaded) {
        std::ifstream file("assets/images/NOTE_assets.xml");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string xmlText = buffer.str();
            file.close();
            
            noteFrames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow("assets/images/NOTE_assets.png", xmlText);
            assetsLoaded = true;
            std::cout << "Note assets loaded successfully" << std::endl;
        } else {
            std::cerr << "Failed to load NOTE_assets.xml" << std::endl;
        }
    }
}

void Note::unloadAssets() {
    if (assetsLoaded) {
        if (noteFrames) {
            delete noteFrames;
            noteFrames = nullptr;
        }
        assetsLoaded = false;
    }
}

Note::Note(float strumTime, int noteData, Note* prevNote, bool sustainNote) 
    : FlxSprite(), strumTime(strumTime), noteData(noteData), prevNote(prevNote), 
      isSustainNote(sustainNote), sustainLength(0), mustPress(false), canBeHit(false),
      tooLate(false), wasGoodHit(false), noteScore(1.0f), parentNote(nullptr) {
    
    if (!assetsLoaded) {
        loadAssets();
    }
    
    x += 42.0f;
    x += 50.0f;
    y -= 2000.0f;
    
    if (noteFrames) {
        texture = noteFrames->texture;
        ownsTexture = false;
        frames = noteFrames;
        animation = new flixel::animation::FlxAnimationController();
        
        if (!noteFrames->frames.empty()) {
            const auto& firstFrame = noteFrames->frames[0];
            sourceRect = firstFrame.rect;
            frameWidth = firstFrame.sourceSize.w;
            frameHeight = firstFrame.sourceSize.h;
            width = static_cast<float>(frameWidth);
            height = static_cast<float>(frameHeight);
        }
        
        std::string noteType;
        if (noteData == LEFT_NOTE) {
            noteType = "purple";
        }
        else if (noteData == DOWN_NOTE) {
            noteType = "blue";
        }
        else if (noteData == UP_NOTE) {
            noteType = "green";
        }
        else if (noteData == RIGHT_NOTE) {
            noteType = "red";
        }
        else {
            noteType = "purple";
            std::cout << "Unknown note type: " << noteData << std::endl;
        }

        if (sustainNote) {
            std::string holdPiecePrefix, holdEndPrefix;
            switch (noteData) {
                case LEFT_NOTE:
                    holdPiecePrefix = "purple hold piece";
                    holdEndPrefix = "pruple end hold";
                    break;
                case DOWN_NOTE:
                    holdPiecePrefix = "blue hold piece";
                    holdEndPrefix = "blue hold end";
                    break;
                case UP_NOTE:
                    holdPiecePrefix = "green hold piece";
                    holdEndPrefix = "green hold end";
                    break;
                case RIGHT_NOTE:
                    holdPiecePrefix = "red hold piece";
                    holdEndPrefix = "red hold end";
                    break;
                default:
                    holdPiecePrefix = "purple hold piece";
                    holdEndPrefix = "pruple end hold";
                    break;
            }
            
            auto scrollFrames = frames->getFramesByPrefix(holdPiecePrefix);
            auto endFrames = frames->getFramesByPrefix(holdEndPrefix);
            if (!scrollFrames.empty()) {
                animation->addByPrefix("hold", scrollFrames, 24, false);
            }
            if (!endFrames.empty()) {
                animation->addByPrefix("hold_end", endFrames, 24, false);
            }
            animation->play("hold");
        } else {
            std::string framePrefix;
            switch (noteData) {
                case LEFT_NOTE:
                    framePrefix = "purple";
                    break;
                case DOWN_NOTE:
                    framePrefix = "blue";
                    break;
                case UP_NOTE:
                    framePrefix = "green";
                    break;
                case RIGHT_NOTE:
                    framePrefix = "red";
                    break;
                default:
                    framePrefix = "purple";
                    break;
            }
            
            auto scrollFrames = frames->getFramesByPrefix(framePrefix);
            if (!scrollFrames.empty()) {
                animation->addByPrefix("scroll", scrollFrames, 24, false);
                animation->play("scroll");
            } else {
                std::cout << "Warning: No frames found for frame prefix: " << framePrefix << std::endl;
            }
        }
    }
    
    x += swagWidth * noteData;

    setScale(0.7f, 0.7f);
    updateHitbox();
    visible = true;
}

void Note::setupNote() {
    std::string colorPrefix;
    switch (noteData) {
        case LEFT_NOTE:
            colorPrefix = "purple";
            break;
        case DOWN_NOTE:
            colorPrefix = "blue";
            break;
        case UP_NOTE:
            colorPrefix = "green";
            break;
        case RIGHT_NOTE:
            colorPrefix = "red";
            break;
    }
}

void Note::setupSustainNote() {
    noteScore *= 0.2f;
    alpha = 0.6f;

    x += width / 2;

    std::string colorPrefix;
    switch (noteData) {
        case LEFT_NOTE:
            colorPrefix = "purple";
            break;
        case DOWN_NOTE:
            colorPrefix = "blue";
            break;
        case UP_NOTE:
            colorPrefix = "green";
            break;
        case RIGHT_NOTE:
            colorPrefix = "red";
            break;
    }

    updateHitbox();
    x -= width / 2;

    if (prevNote && prevNote->isSustainNote) {
        prevNote->scaleY *= static_cast<float>(Conductor::stepCrochet) / 100.0f * 1.5f * 1.0f;
        prevNote->updateHitbox();
    }
}

float Note::getTargetY() {
    int windowHeight = 720;
    if (GameConfig::getInstance()->isDownscroll()) {
        return windowHeight - 150.0f;
    }
    return 50.0f;
}

void Note::update(float elapsed) {
    FlxSprite::update(elapsed);

    if (animation) {
        animation->update(elapsed);
    }

    float songPos = Conductor::songPosition;
    float scrollSpeed = PlayState::SONG.speed;
    
    float targetY = getTargetY();
    
    float timeDiff = strumTime - songPos;
    float distance = timeDiff * 0.45f * scrollSpeed;
    
    float x = this->x;
    float y = targetY + (GameConfig::getInstance()->isDownscroll() ? -distance : distance) + yOffset;
    
    setPosition(x, y);
    visible = true;
    
    if (isSustainNote && y + offsetY <= targetY + swagWidth / 2.0f &&
        (!mustPress || (wasGoodHit || (prevNote && prevNote->wasGoodHit && !canBeHit)))) {
        float clipY = targetY + swagWidth / 2.0f - y;
        clipRect.x = 0;
        clipRect.y = static_cast<int>(clipY / getScaleY());
        clipRect.w = static_cast<int>(width * 2.0f);
        clipRect.h = static_cast<int>(height * 2.0f) - clipRect.y;
        
        if (clipRect.h > 0) {
            useClipRect = true;
        } else {
            useClipRect = false;
        }
    } else {
        useClipRect = false;
    }

    if (mustPress) {
        if (strumTime > songPos - Conductor::safeZoneOffset &&
            strumTime < songPos + (Conductor::safeZoneOffset * 0.5f)) {
            canBeHit = true;
        } else {
            canBeHit = false;
        }

        if (strumTime < songPos - Conductor::safeZoneOffset && !wasGoodHit) {
            tooLate = true;
        }
    } else {
        canBeHit = false;
        if (strumTime <= songPos) {
            wasGoodHit = true;
        }
    }
}

Note::~Note() {}