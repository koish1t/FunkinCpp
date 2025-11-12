#pragma once

#include <string>

class Scoring {
public:
    struct Judgement {
        std::string rating;
        int score;
        float accuracy;
    };
    
    static Judgement judgeNote(float noteDiff, float safeZoneOffset);
    static float calculateAccuracy(int sicks, int goods, int bads, int shits, int misses);
};

