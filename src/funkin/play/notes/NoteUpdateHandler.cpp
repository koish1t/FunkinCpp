#include "NoteUpdateHandler.h"
#include "../song/Conductor.h"

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
                if (gameplayManager && boyfriend) {
                    gameplayManager->noteMiss(note->noteData, boyfriend, vocals);
                }
                if (noteHitHandler) {
                    noteHitHandler->incrementMisses();
                    noteHitHandler->resetCombo();
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