#pragma once

#include "NoteSprite.h"
#include "../song/Song.h"
#include "../song/Conductor.h"
#include <vector>
#include <flixel/FlxCamera.h>

class NoteManager {
public:
    NoteManager(flixel::FlxCamera* hudCamera);
    ~NoteManager();
    
    void generateNotes(const SwagSong& song);
    void updateSpawning(float songPosition);
    void clear();
    
    std::vector<NoteSprite*>& getActiveNotes() { return activeNotes; }
    std::vector<NoteSprite*>& getUnspawnedNotes() { return unspawnedNotes; }
    
private:
    std::vector<NoteSprite*> activeNotes;
    std::vector<NoteSprite*> unspawnedNotes;
    flixel::FlxCamera* hudCamera;
    
    NoteSprite* createNote(float strumTime, int noteType, float sustainLength, bool mustPress);
};