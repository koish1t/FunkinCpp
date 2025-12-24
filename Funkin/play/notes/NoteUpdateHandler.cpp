#include "NoteUpdateHandler.h"
#include "../song/Conductor.h"
#include "../../scripting/ScriptManager.h"
#include "../../game/GameConfig.h"

NoteUpdateHandler::NoteUpdateHandler(NoteManager* noteManager, NoteHitHandler* noteHitHandler,
                                     GameplayManager* gameplayManager, Character* gf)
    : noteManager(noteManager)
    , noteHitHandler(noteHitHandler)
    , gameplayManager(gameplayManager)
    , gf(gf)
{
}

void NoteUpdateHandler::updateNotes(float elapsed, Character* boyfriend, flixel::FlxSound* vocals) {
    if (!noteManager) return;
    
    noteManager->updateSpawning(Conductor::songPosition);
    
    auto& notes = noteManager->getActiveNotes();
    for (auto it = notes.begin(); it != notes.end();) {
        NoteSprite* note = *it;
        if (note) {
            note->update(elapsed);

            if (note->mustPress && note->tooLate && !note->wasGoodHit) {
                ScriptManager::getInstance()->callAll(ScriptCallback::ON_NOTE_MISS, {note->noteData});
                
                if (gameplayManager && boyfriend) {
                    gameplayManager->noteMiss(note->noteData, boyfriend, vocals);
                }
                if (noteHitHandler) {
                    noteHitHandler->incrementMisses();
                    noteHitHandler->resetCombo();
                    noteHitHandler->updateScore();
                }
                if (gf && noteHitHandler && noteHitHandler->getCombo() > 5) {
                    gf->playAnim("sad", true);
                }
                note->kill = true;
            }

            if (note->wasGoodHit && note->sustainLength > 0) {
                float sustainEndTime = note->strumTime + note->sustainLength;
                if (Conductor::songPosition >= sustainEndTime) {
                    note->kill = true;
                } else if (note->holdReleased) {
                    float timeSinceRelease = Conductor::songPosition - note->hitTime - 
                        (note->sustainLength - (sustainEndTime - Conductor::songPosition));
                    if (timeSinceRelease > 200.0f) {
                        note->kill = true;
                    }
                }
            }

            if (note->kill || note->strumTime < Conductor::songPosition - 5000) {
                it = notes.erase(it);
                delete note;
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}