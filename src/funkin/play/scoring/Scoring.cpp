#include "Scoring.h"
#include <cmath>

Scoring::Judgement Scoring::judgeNote(float noteDiff, float safeZoneOffset) {
    Judgement result;
    
    float absNoteDiff = std::abs(noteDiff);
    
    if (absNoteDiff > safeZoneOffset * 0.9f) {
        result.rating = "shit";
        result.score = 50;
        result.accuracy = 0.25f;
    }
    else if (absNoteDiff > safeZoneOffset * 0.75f) {
        result.rating = "bad";
        result.score = 100;
        result.accuracy = 0.5f;
    }
    else if (absNoteDiff > safeZoneOffset * 0.2f) {
        result.rating = "good";
        result.score = 200;
        result.accuracy = 0.75f;
    }
    else {
        result.rating = "sick";
        result.score = 350;
        result.accuracy = 1.0f;
    }
    
    return result;
}

float Scoring::calculateAccuracy(int sicks, int goods, int bads, int shits, int misses) {
    int totalNotes = sicks + goods + bads + shits + misses;
    if (totalNotes == 0) {
        return 0.0f;
    }
    
    float accuracy = ((sicks * 1.0f + goods * 0.75f + bads * 0.5f + shits * 0.25f) / totalNotes) * 100.0f;
    return accuracy;
}