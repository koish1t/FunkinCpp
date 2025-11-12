#include "CharacterManager.h"

CharacterManager::CharacterManager(Character* gf, Character* dad, Character* boyfriend, int gfSpeed)
    : gf(gf)
    , dad(dad)
    , boyfriend(boyfriend)
    , gfSpeed(gfSpeed)
{
}

void CharacterManager::update(float elapsed) {
    if (gf) {
        gf->update(elapsed);
    }
    if (dad) {
        dad->update(elapsed);
    }
    if (boyfriend) {
        boyfriend->update(elapsed);
    }
}

void CharacterManager::beatHit(int curBeat, int curStep, const SwagSong& song) {
    if (gf && curBeat % gfSpeed == 0) {
        gf->dance();
    }
    
    if (boyfriend && boyfriend->animation) {
        std::string currentAnim = boyfriend->animation->current;
        bool isSinging = (currentAnim.length() >= 4 && currentAnim.substr(0, 4) == "sing");
        bool isSpecialAnim = (currentAnim == "hey" || currentAnim == "scared" || currentAnim == "firstDeath" || 
                             currentAnim == "deathLoop" || currentAnim == "deathConfirm");
        
        if (!isSinging && !isSpecialAnim) {
            boyfriend->dance();
        }
    }
    
    if (dad && curBeat % 1 == 0) {
        int curSection = curStep / 16;
        if (curSection >= 0 && curSection < song.notes.size()) {
            if (song.notes[curSection].mustHitSection) {
                dad->dance();
            }
        }
    }
}