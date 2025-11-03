#include "NoteHitHandler.h"
#include "../song/Conductor.h"
#include "../../game/GameConfig.h"
#include <flixel/FlxG.h>
#include <cmath>
#include <algorithm>

NoteHitHandler::NoteHitHandler(Controls* controls, NoteManager* noteManager,
                               Strumline* playerStrumline, Character* boyfriend,
                               HealthBar* healthBar, PopUpStuff* popUpStuff,
                               flixel::FlxText* scoreText, flixel::FlxSound* vocals,
                               flixel::FlxCamera* camHUD)
    : controls(controls)
    , noteManager(noteManager)
    , playerStrumline(playerStrumline)
    , boyfriend(boyfriend)
    , healthBar(healthBar)
    , popUpStuff(popUpStuff)
    , scoreText(scoreText)
    , vocals(vocals)
    , camHUD(camHUD)
    , score(0)
    , misses(0)
    , combo(0)
    , sicks(0)
    , goods(0)
    , bads(0)
    , shits(0)
{
}

void NoteHitHandler::handleInput() {
    if (!controls || !noteManager) return;
    
    auto& notes = noteManager->getActiveNotes();
    
    float closestDistances[4] = {INFINITY, INFINITY, INFINITY, INFINITY};
    std::vector<NoteSprite*> toHit;
    
    bool justHitArray[4] = {
        controls->justPressed(0),
        controls->justPressed(1),
        controls->justPressed(2),
        controls->justPressed(3)
    };
    
    bool heldArray[4] = {
        controls->pressed(0),
        controls->pressed(1),
        controls->pressed(2),
        controls->pressed(3)
    };
    
    if (boyfriend && (justHitArray[0] || justHitArray[1] || justHitArray[2] || justHitArray[3])) {
        boyfriend->holdTimer = 0.0f;
    }
    
    for (int i = 0; i < 4; i++) {
        if (justHitArray[i] && playerStrumline) {
            playerStrumline->playPressed(i);
        }
    }
    
    for (auto note : notes) {
        if (!note || note->kill || note->wasGoodHit || note->tooLate) {
            continue;
        }
        
        if (!note->mustPress || !note->canBeHit) {
            continue;
        }
        
        int lane = note->noteData;
        if (lane < 0 || lane >= 4) {
            continue;
        }
        
        if (note->isSustainNote) {
            if (heldArray[lane] && note->parentNote && note->parentNote->wasGoodHit) {
                goodNoteHit(note);
            }
            continue;
        }
        
        if (!justHitArray[lane]) {
            continue;
        }
        
        float rawHitTime = note->strumTime - Conductor::songPosition;
        float distance = std::abs(rawHitTime);
        
        float& closestLaneDistance = closestDistances[lane];
        
        if (closestLaneDistance != INFINITY && std::abs(closestLaneDistance - distance) > 5.0f) {
            continue;
        }
        
        closestLaneDistance = distance;
        
        toHit.erase(std::remove_if(toHit.begin(), toHit.end(), [lane, distance](NoteSprite* n) {
            return n->noteData == lane && std::abs(n->strumTime - Conductor::songPosition) > distance;
        }), toHit.end());
        
        toHit.push_back(note);
    }
    
    for (auto note : toHit) {
        goodNoteHit(note);
    }
    
    if (!GameConfig::getInstance()->isGhostTapping()) {
        for (int i = 0; i < 4; i++) {
            if (justHitArray[i] && closestDistances[i] == INFINITY) {
            }
        }
    }
    
    for (int i = 0; i < 4; i++) {
        if (controls->justReleased(i) && playerStrumline) {
            playerStrumline->playStatic(i);
        }
    }
}

void NoteHitHandler::goodNoteHit(NoteSprite* note) {
    if (!note->wasGoodHit) {
        note->wasGoodHit = true;
        
        if (vocals) {
            vocals->setVolume(1.0f);
        }
        
        if (note->noteData >= 0 && note->noteData < 4) {
            if (playerStrumline) {
                playerStrumline->playNote(note->noteData, true);
            }
            
            if (boyfriend) {
                std::string animToPlay = "";
                switch (note->noteData) {
                    case 0: animToPlay = "singLEFT"; break;
                    case 1: animToPlay = "singDOWN"; break;
                    case 2: animToPlay = "singUP"; break;
                    case 3: animToPlay = "singRIGHT"; break;
                }
                if (!animToPlay.empty()) {
                    boyfriend->playAnim(animToPlay, true);
                    boyfriend->holdTimer = 0.0f;
                }
            }
        }

        if (!note->isSustainNote) {
            float noteDiff = note->strumTime - Conductor::songPosition;
            float safeZoneOffset = (10.0f / 60.0f) * 1000.0f;
            
            Scoring::Judgement judgement = Scoring::judgeNote(noteDiff, safeZoneOffset);
            
            if (judgement.rating == "shit") {
                shits++;
            } else if (judgement.rating == "bad") {
                bads++;
            } else if (judgement.rating == "good") {
                goods++;
            } else {
                sicks++;
            }
            
            if (healthBar) {
                healthBar->setHealth(healthBar->getHealth() + 0.05f);
            }
            combo++;
            score += judgement.score;
            
            if (popUpStuff) {
                popUpStuff->popUpScore(judgement.rating, combo, Conductor::crochet, camHUD);
            }
            
            updateScoreText();
            note->kill = true;
        }
    }
}

void NoteHitHandler::updateScoreText() {
    if (!scoreText) return;
    
    std::string text = "Score: " + std::to_string(score) + 
                      " | Misses: " + std::to_string(misses) + 
                      " | Accuracy: ";
    
    float accuracy = Scoring::calculateAccuracy(sicks, goods, bads, shits, misses);
    char accStr[16];
    snprintf(accStr, sizeof(accStr), "%.2f%%", accuracy);
    text += accStr;
    
    scoreText->setText(text);
}