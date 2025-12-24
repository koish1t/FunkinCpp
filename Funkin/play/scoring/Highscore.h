#pragma once

#include <string>
#include <map>

class Highscore {
public:
    static std::map<std::string, int> songScores;
    static std::map<std::string, float> songAccuracies;
    
    static void saveScore(const std::string& song, int score = 0, int diff = 0, float accuracy = 0.0f);
    static void saveWeekScore(int week = 1, int score = 0, int diff = 0, float accuracy = 0.0f);
    static int getScore(const std::string& song, int diff);
    static float getAccuracy(const std::string& song, int diff);
    static int getWeekScore(int week, int diff);
    static std::string formatSong(const std::string& song, int diff);
    static void load();
    static void save();
    static void printAllAccuracies();
    
private:
    static void setScore(const std::string& song, int score);
    static void setAccuracy(const std::string& song, float accuracy);
};