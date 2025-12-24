#pragma once
#include <vector>
#include "Song.h"

struct BPMChangeEvent {
    int stepTime;
    float songTime;
    float bpm;
};

class Conductor {
public:
    static float bpm;
    static float crochet; // beats in milliseconds
    static float stepCrochet; // steps in milliseconds
    static float songPosition;
    static float lastSongPos;
    static float offset;

    static int safeFrames;
    static float safeZoneOffset; // is calculated in create(), is safeFrames in milliseconds

    static std::vector<BPMChangeEvent> bpmChangeMap;

    Conductor();
    
    static void mapBPMChanges(const SwagSong& song);
    static void changeBPM(float newBpm, float songMultiplier = 1.0f);
    static void recalculateStuff(float songMultiplier = 1.0f);
};
