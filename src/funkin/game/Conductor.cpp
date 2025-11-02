#include "Conductor.h"
#include <iostream>
#include "Song.h"

float Conductor::bpm = 0;
float Conductor::crochet = 0;
float Conductor::stepCrochet = 0;
float Conductor::songPosition = 0;
float Conductor::lastSongPos = 0;
float Conductor::offset = 0;
float Conductor::safeZoneOffset = 0;

int Conductor::safeFrames = 10;

std::vector<BPMChangeEvent> Conductor::bpmChangeMap;

Conductor::Conductor() {
}

void Conductor::mapBPMChanges(const SwagSong& song) {
    bpmChangeMap.clear();

    float curBPM = song.bpm;
    int totalSteps = 0;
    float totalPos = 0.0f;

    for (size_t i = 0; i < song.notes.size(); i++) {
        if (song.notes[i].changeBPM && song.notes[i].bpm != curBPM) {
            curBPM = song.notes[i].bpm;
            BPMChangeEvent event{
                totalSteps,
                totalPos,
                static_cast<int>(curBPM)
            };
            bpmChangeMap.push_back(event);
        }

        int deltaSteps = song.notes[i].lengthInSteps;
        totalSteps += deltaSteps;
        totalPos += ((60.0f / curBPM) * 1000.0f / 4.0f) * deltaSteps;
    }

    std::cout << "new BPM map BUDDY ";
    for (const auto& event : bpmChangeMap) {
        std::cout << "{ stepTime: " << event.stepTime 
                  << ", songTime: " << event.songTime 
                  << ", bpm: " << event.bpm << " } ";
    }
    std::cout << std::endl;
}

void Conductor::changeBPM(float newBpm, float songMultiplier) {
    bpm = newBpm;
    recalculateStuff(songMultiplier);
}

void Conductor::recalculateStuff(float songMultiplier) {
    crochet = ((60.0f / bpm) * 1000.0f);
    stepCrochet = crochet / 4.0f;
    safeZoneOffset = (safeFrames / 60.0f) * 1000.0f;
}
