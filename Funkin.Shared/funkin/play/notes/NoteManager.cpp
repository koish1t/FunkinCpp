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
                
                NoteSprite* note = createNote(strumTime, noteType, sustainLength, mustPress);
                unspawnedNotes.push_back(note);
                totalNotes++;
            }
        }
        currentSection++;
    }
    
    std::sort(unspawnedNotes.begin(), unspawnedNotes.end(),
        [](NoteSprite* a, NoteSprite* b) {
            return a->strumTime < b->strumTime;
        });
    
    std::cout << "Generated " << totalNotes << " notes" << std::endl;
}

NoteSprite* NoteManager::createNote(float strumTime, int noteType, float sustainLength, bool mustPress) {
    NoteSprite* note = new NoteSprite(strumTime, noteType);
    note->sustainLength = sustainLength;
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