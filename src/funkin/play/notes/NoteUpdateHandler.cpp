#include "NoteUpdateHandler.h"
#include "../song/Conductor.h"
#include "../../scripting/ScriptManager.h"

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

            if (note->y < -note->height) {
                if (note->isSustainNote && note->wasGoodHit) {
                    note->kill = true;
                }
            }

            if (note->kill || note->strumTime < Conductor::songPosition - 5000) {
                it = notes.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}