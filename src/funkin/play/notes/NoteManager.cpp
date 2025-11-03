#include "NoteManager.h"
#include <flixel/FlxG.h>
#include <iostream>
#include <algorithm>
#include <cmath>

NoteManager::NoteManager(flixel::FlxCamera* hudCamera)
    : hudCamera(hudCamera)
{
}

NoteManager::~NoteManager() {
    clear();
}

void NoteManager::generateNotes(const SwagSong& song) {
    clear();
    
    if (song.notes.empty()) {
        return;
    }
    
    std::cout << "Generating notes from " << song.notes.size() << " sections" << std::endl;
    
    int totalNotes = 0;
    int currentSection = 0;
    
    for (const auto& section : song.notes) {
        for (const auto& noteData : section.sectionNotes) {
            if (noteData.size() >= 2) {
                float strumTime = noteData[0];
                int noteType = static_cast<int>(noteData[1]);
                
                if (noteType < 0 || noteType > 7) {
                    std::cout << "Warning: Invalid note type " << noteType << " in section " << currentSection << std::endl;
                    continue;
                }
                
                bool mustPress = section.mustHitSection;
                if (noteType >= 4) {
                    mustPress = !section.mustHitSection;
                    noteType = noteType % 4;
                }
                
                float sustainLength = (noteData.size() > 2) ? noteData[2] : 0;
                
                NoteSprite* oldNote = !unspawnedNotes.empty() ? unspawnedNotes.back() : nullptr;
                NoteSprite* swagNote = createNote(strumTime, noteType, oldNote, mustPress);
                swagNote->sustainLength = sustainLength;
                
                if (sustainLength > 0) {
                    float susLength = sustainLength / Conductor::stepCrochet;
                    int numSustainPieces = static_cast<int>(std::round(susLength));
                    
                    for (int susNote = 0; susNote < numSustainPieces; susNote++) {
                        oldNote = !unspawnedNotes.empty() ? unspawnedNotes.back() : nullptr;
                        float sustainNoteTime = strumTime + (Conductor::stepCrochet * susNote);
                        
                        NoteSprite* sustainNote = createSustainNote(sustainNoteTime, noteType, oldNote, 
                                                                     mustPress, swagNote, susNote, 
                                                                     numSustainPieces, song.speed);
                        unspawnedNotes.push_back(sustainNote);
                        totalNotes++;
                    }
                }
                
                unspawnedNotes.push_back(swagNote);
                totalNotes++;
            }
        }
        currentSection++;
    }
    
    std::sort(unspawnedNotes.begin(), unspawnedNotes.end(),
        [](NoteSprite* a, NoteSprite* b) {
            return a->strumTime < b->strumTime;
        });
}

NoteSprite* NoteManager::createNote(float strumTime, int noteType, NoteSprite* prevNote, bool mustPress) {
    NoteSprite* note = new NoteSprite(strumTime, noteType, prevNote, false);
    note->mustPress = mustPress;
    note->scrollFactor.x = 0.0f;
    note->scrollFactor.y = 0.0f;
    
    if (mustPress) {
        note->x += static_cast<float>(flixel::FlxG::width) / 2.0f;
    }
    
    if (hudCamera) {
        note->camera = hudCamera;
    }
    
    return note;
}

NoteSprite* NoteManager::createSustainNote(float strumTime, int noteType, NoteSprite* prevNote, 
                                            bool mustPress, NoteSprite* parentNote, int pieceIndex, 
                                            int totalPieces, float speed) {
    NoteSprite* sustainNote = new NoteSprite(strumTime, noteType, prevNote, true);
    sustainNote->mustPress = mustPress;
    sustainNote->scrollFactor.x = 0.0f;
    sustainNote->scrollFactor.y = 0.0f;
    sustainNote->parentNote = parentNote;
    
    if (pieceIndex == totalPieces - 1 && sustainNote->animation) {
        sustainNote->animation->play("hold_end");
        sustainNote->setScale(0.7f, 0.7f);
        sustainNote->updateHitbox();
        sustainNote->x += 34.0f;
    } else {
        float scaleMultiplier = Conductor::stepCrochet / 100.0f * 1.5f * speed;
        sustainNote->setScale(0.7f, 0.7f * scaleMultiplier);
        sustainNote->originX = 0.0f;
        sustainNote->originY = 0.0f;
        sustainNote->x += 34.0f;
    }
    
    sustainNote->yOffset = 30.0f;
    
    if (mustPress) {
        sustainNote->x += static_cast<float>(flixel::FlxG::width) / 2.0f;
    }
    
    if (hudCamera) {
        sustainNote->camera = hudCamera;
    }
    
    return sustainNote;
}

void NoteManager::updateSpawning(float songPosition) {
    while (!unspawnedNotes.empty()) {
        NoteSprite* nextNote = unspawnedNotes[0];
        if (nextNote->strumTime - songPosition > 1500) {
            break;
        }
        
        activeNotes.push_back(nextNote);
        unspawnedNotes.erase(unspawnedNotes.begin());
    }
}

void NoteManager::clear() {
    for (auto note : activeNotes) {
        delete note;
    }
    activeNotes.clear();
    
    for (auto note : unspawnedNotes) {
        delete note;
    }
    unspawnedNotes.clear();
}