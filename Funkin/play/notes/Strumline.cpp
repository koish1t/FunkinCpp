#include "Strumline.h"
#include "NoteSprite.h"
#include <flixel/FlxG.h>
#include <flixel/animation/FlxAnimationController.h>
#include <flixel/tweens/FlxTweenUtil.h>
#include <iostream>

Strumline::Strumline(float x, float y, int player, flixel::FlxCamera* camera)
    : x(x), y(y), player(player), camera(camera)
{
    if (!NoteSprite::noteFrames) {
        std::cerr << "ERROR: Note assets not loaded!" << std::endl;
        return;
    }
    
    for (int i = 0; i < 4; i++) {
        float xPos = x + (NoteSprite::swagWidth * i);
        createNote(i, xPos, y);
    }
}

Strumline::~Strumline() {
    for (auto note : notes) {
        if (note) {
            delete note;
        }
    }
    notes.clear();
}

void Strumline::createNote(int i, float xPos, float yPos) {
    flixel::FlxSprite* babyArrow = new flixel::FlxSprite();
    babyArrow->setPosition(xPos, yPos);
    
    babyArrow->texture = NoteSprite::noteFrames->texture;
    babyArrow->ownsTexture = false;
    babyArrow->frames = NoteSprite::noteFrames;
    babyArrow->animation = new flixel::animation::FlxAnimationController();
    
    if (!NoteSprite::noteFrames->frames.empty()) {
        const auto& firstFrame = NoteSprite::noteFrames->frames[0];
        babyArrow->sourceRect = firstFrame.rect;
        babyArrow->frameWidth = firstFrame.sourceSize.w;
        babyArrow->frameHeight = firstFrame.sourceSize.h;
        babyArrow->width = static_cast<float>(babyArrow->frameWidth);
        babyArrow->height = static_cast<float>(babyArrow->frameHeight);
    }
    
    std::string staticFrame, pressPrefix, confirmPrefix;
    switch (i) {
        case 0:
            staticFrame = "arrowLEFT";
            pressPrefix = "left press";
            confirmPrefix = "left confirm";
            break;
        case 1:
            staticFrame = "arrowDOWN";
            pressPrefix = "down press";
            confirmPrefix = "down confirm";
            break;
        case 2:
            staticFrame = "arrowUP";
            pressPrefix = "up press";
            confirmPrefix = "up confirm";
            break;
        case 3:
            staticFrame = "arrowRIGHT";
            pressPrefix = "right press";
            confirmPrefix = "right confirm";
            break;
    }
    
    auto staticFrames = NoteSprite::noteFrames->getFramesByPrefix(staticFrame);
    auto pressedFrames = NoteSprite::noteFrames->getFramesByPrefix(pressPrefix);
    auto confirmFrames = NoteSprite::noteFrames->getFramesByPrefix(confirmPrefix);
    
    if (!staticFrames.empty()) {
        babyArrow->animation->addByPrefix("static", staticFrames, 24, false);
    }
    if (!pressedFrames.empty()) {
        babyArrow->animation->addByPrefix("pressed", pressedFrames, 24, false);
    }
    if (!confirmFrames.empty()) {
        babyArrow->animation->addByPrefix("confirm", confirmFrames, 24, false);
    }
    
    babyArrow->setScale(0.7f, 0.7f);
    babyArrow->updateHitbox();
    babyArrow->animation->play("static");
    babyArrow->visible = true;
    babyArrow->scrollFactor.x = 0.0f;
    babyArrow->scrollFactor.y = 0.0f;
    
    if (camera) {
        babyArrow->camera = camera;
    }
    
    notes.push_back(babyArrow);
}

void Strumline::update(float elapsed) {
    for (auto note : notes) {
        if (note && note->animation) {
            note->animation->update(elapsed);
        }
    }
    
    for (auto note : notes) {
        if (note && note->animation && note->animation->current == "confirm" && note->animation->finished) {
            note->animation->play("static");
        }
    }
    
    for (auto note : notes) {
        if (note && note->frames && note->animation) {
            int frameIdx = note->animation->getCurrentFrame();
            if (frameIdx >= 0 && frameIdx < static_cast<int>(note->frames->frames.size())) {
                const auto& frame = note->frames->frames[frameIdx];
                note->frameWidth = frame.sourceSize.w;
                note->frameHeight = frame.sourceSize.h;
            }
            
            if (note->animation->current == "confirm") {
                note->centerOffsets();
                note->offsetX -= 13.0f;
                note->offsetY -= 13.0f;
            } else {
                note->centerOffsets();
            }
        }
    }
}

void Strumline::draw() {
    for (auto note : notes) {
        if (note && note->visible) {
            note->draw();
        }
    }
}

void Strumline::playNote(int direction, bool confirm) {
    if (direction >= 0 && direction < notes.size() && notes[direction]) {
        if (notes[direction]->animation) {
            if (confirm) {
                if (notes[direction]->animation->current != "confirm") {
                    notes[direction]->animation->play("confirm");
                }
            } else {
                notes[direction]->animation->play("pressed");
            }
        }
    }
}

void Strumline::playStatic(int direction) {
    if (direction >= 0 && direction < notes.size() && notes[direction]) {
        if (notes[direction]->animation && notes[direction]->animation->current != "confirm") {
            notes[direction]->animation->play("static");
        }
    }
}

void Strumline::playPressed(int direction) {
    if (direction >= 0 && direction < notes.size() && notes[direction]) {
        if (notes[direction]->animation && notes[direction]->animation->current != "confirm") {
            notes[direction]->animation->play("pressed");
        }
    }
}

flixel::FlxSprite* Strumline::getNote(int index) {
    if (index >= 0 && index < notes.size()) {
        return notes[index];
    }
    return nullptr;
}

void Strumline::fadeInArrow(int index, flixel::FlxSprite* arrow) {
    if (!arrow) return;
    
    arrow->y -= 10.0f;
    arrow->alpha = 0.0f;
    
    float targetY = arrow->y + 10.0f;
    float duration = 1.0f;
    float startDelay = 0.5f + (0.2f * index);
    
    flixel::tweens::tweenY(arrow, targetY, duration, flixel::tweens::FlxEase::circOut, nullptr, startDelay);
    flixel::tweens::tweenAlpha(arrow, 1.0f, duration, flixel::tweens::FlxEase::circOut, nullptr, startDelay);
}

void Strumline::fadeInArrows() {
    for (int i = 0; i < static_cast<int>(notes.size()); i++) {
        fadeInArrow(i, notes[i]);
    }
}