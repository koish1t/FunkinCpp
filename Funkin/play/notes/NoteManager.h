#pragma once

#include "NoteSprite.h"
#include "../song/Song.h"
#include "../song/Conductor.h"
#include <vector>
#include <flixel/FlxCamera.h>

struct CachedNoteData {
    float strumTime;
    int noteType;
    float sustainLength;
    bool mustPress;
};

class NoteManager {
public:
    NoteManager(flixel::FlxCamera* hudCamera);
    ~NoteManager();
    
    void generateNotes(const SwagSong& song);
    void regenerateFromCache();
    void updateSpawning(float songPosition);
    void clear();
    
    std::vector<NoteSprite*>& getActiveNotes() { return activeNotes; }
    std::vector<NoteSprite*>& getUnspawnedNotes() { return unspawnedNotes; }
    
    bool hasCachedNotes() const { return !cachedNotes.empty(); }
    const std::vector<CachedNoteData>& getCachedNotes() const { return cachedNotes; }
    void setCachedNotes(const std::vector<CachedNoteData>& notes) { cachedNotes = notes; }
    void returnToPool(NoteSprite* note);
    
private:
    std::vector<NoteSprite*> activeNotes;
    std::vector<NoteSprite*> unspawnedNotes;
    std::vector<CachedNoteData> cachedNotes;
    static std::vector<NoteSprite*> notePool;
    flixel::FlxCamera* hudCamera;
    
    NoteSprite* createNote(float strumTime, int noteType, float sustainLength, bool mustPress);
    NoteSprite* getPooledNote(float strumTime, int noteType, float sustainLength, bool mustPress);
};