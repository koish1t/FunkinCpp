#pragma once
#include <vector>

struct SwagSection {
    std::vector<std::vector<float>> sectionNotes;
    int lengthInSteps = 16;
    int typeOfSection = 0;
    bool mustHitSection = true;
    int bpm = 0;
    bool changeBPM = false;
    bool altAnim = false;
};

class Section {
public:
    std::vector<std::vector<float>> sectionNotes;
    int lengthInSteps = 16;
    int typeOfSection = 0;
    bool mustHitSection = true;
    static const int COPYCAT = 0;
    explicit Section(int lengthInSteps = 16);
};
