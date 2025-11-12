#pragma once

#include "ScriptTypes.h"
#include <string>
#include <map>

namespace flixel {
    class FlxSprite;
    class FlxText;
    namespace tweens {
        class FlxTween;
    }
    namespace util {
        class FlxTimer;
    }
}

class ScriptAPI {
public:
    static void initialize();
    static void cleanup();
    static ScriptValue callFunction(const std::string& funcName, const std::vector<ScriptValue>& args);
    static bool hasFunction(const std::string& funcName);

    static const std::map<std::string, flixel::FlxSprite*>& getSprites() { return sprites; }
    static const std::map<std::string, flixel::FlxText*>& getTexts() { return texts; }

private:
    static std::map<std::string, ScriptFunction> functions;
    static std::map<std::string, flixel::FlxSprite*> sprites;
    static std::map<std::string, flixel::FlxText*> texts;
    static std::map<std::string, flixel::tweens::FlxTween*> tweens;
    static std::map<std::string, flixel::util::FlxTimer*> timers;

    static int getInt(const ScriptValue& val);
    static float getFloat(const ScriptValue& val);
    static bool getBool(const ScriptValue& val);
    static std::string getString(const ScriptValue& val);

    static ScriptValue setHealth(const std::vector<ScriptValue>& args);
    static ScriptValue getHealth(const std::vector<ScriptValue>& args);
    static ScriptValue setScore(const std::vector<ScriptValue>& args);
    static ScriptValue getScore(const std::vector<ScriptValue>& args);
    static ScriptValue addScore(const std::vector<ScriptValue>& args);
    static ScriptValue getMisses(const std::vector<ScriptValue>& args);
    static ScriptValue setMisses(const std::vector<ScriptValue>& args);
    static ScriptValue getCombo(const std::vector<ScriptValue>& args);
    static ScriptValue setCombo(const std::vector<ScriptValue>& args);
    static ScriptValue getSicks(const std::vector<ScriptValue>& args);
    static ScriptValue getGoods(const std::vector<ScriptValue>& args);
    static ScriptValue getBads(const std::vector<ScriptValue>& args);
    static ScriptValue getShits(const std::vector<ScriptValue>& args);

    static ScriptValue playCharAnim(const std::vector<ScriptValue>& args);
    static ScriptValue setCharPos(const std::vector<ScriptValue>& args);
    static ScriptValue getCharPos(const std::vector<ScriptValue>& args);
    static ScriptValue getCharX(const std::vector<ScriptValue>& args);
    static ScriptValue getCharY(const std::vector<ScriptValue>& args);
    static ScriptValue setCharVisible(const std::vector<ScriptValue>& args);

    static ScriptValue setCameraPos(const std::vector<ScriptValue>& args);
    static ScriptValue setCameraZoom(const std::vector<ScriptValue>& args);
    static ScriptValue getCameraZoom(const std::vector<ScriptValue>& args);
    static ScriptValue cameraShake(const std::vector<ScriptValue>& args);
    static ScriptValue cameraFlash(const std::vector<ScriptValue>& args);
    static ScriptValue cameraFade(const std::vector<ScriptValue>& args);

    static ScriptValue playSound(const std::vector<ScriptValue>& args);
    static ScriptValue playMusic(const std::vector<ScriptValue>& args);
    static ScriptValue stopSound(const std::vector<ScriptValue>& args);
    static ScriptValue pauseSound(const std::vector<ScriptValue>& args);
    static ScriptValue resumeSound(const std::vector<ScriptValue>& args);
    static ScriptValue setSoundVolume(const std::vector<ScriptValue>& args);
    static ScriptValue getSoundVolume(const std::vector<ScriptValue>& args);
    static ScriptValue getSoundTime(const std::vector<ScriptValue>& args);

    static ScriptValue makeSprite(const std::vector<ScriptValue>& args);
    static ScriptValue makeSpriteGraphic(const std::vector<ScriptValue>& args);
    static ScriptValue setSpritePos(const std::vector<ScriptValue>& args);
    static ScriptValue setSpriteScale(const std::vector<ScriptValue>& args);
    static ScriptValue setSpriteVisible(const std::vector<ScriptValue>& args);
    static ScriptValue setSpriteAlpha(const std::vector<ScriptValue>& args);
    static ScriptValue setSpriteAngle(const std::vector<ScriptValue>& args);
    static ScriptValue removeSprite(const std::vector<ScriptValue>& args);

    static ScriptValue makeText(const std::vector<ScriptValue>& args);
    static ScriptValue setTextString(const std::vector<ScriptValue>& args);
    static ScriptValue setTextSize(const std::vector<ScriptValue>& args);
    static ScriptValue setTextColor(const std::vector<ScriptValue>& args);
    static ScriptValue setTextFont(const std::vector<ScriptValue>& args);

    static ScriptValue doTween(const std::vector<ScriptValue>& args);
    static ScriptValue cancelTween(const std::vector<ScriptValue>& args);

    static ScriptValue startTimer(const std::vector<ScriptValue>& args);
    static ScriptValue cancelTimer(const std::vector<ScriptValue>& args);

    static ScriptValue debugPrint(const std::vector<ScriptValue>& args);
    static ScriptValue getProperty(const std::vector<ScriptValue>& args);
    static ScriptValue setProperty(const std::vector<ScriptValue>& args);
    static ScriptValue getRandomInt(const std::vector<ScriptValue>& args);
    static ScriptValue getRandomFloat(const std::vector<ScriptValue>& args);

    static ScriptValue getSongPosition(const std::vector<ScriptValue>& args);
    static ScriptValue getSongLength(const std::vector<ScriptValue>& args);
    static ScriptValue getCurrentBeat(const std::vector<ScriptValue>& args);
    static ScriptValue getCurrentStep(const std::vector<ScriptValue>& args);
    static ScriptValue getBPM(const std::vector<ScriptValue>& args);
    static ScriptValue getCrochet(const std::vector<ScriptValue>& args);
    static ScriptValue getStepCrochet(const std::vector<ScriptValue>& args);

    static ScriptValue getStoryMode(const std::vector<ScriptValue>& args);
    static ScriptValue getDifficulty(const std::vector<ScriptValue>& args);
    static ScriptValue getSongName(const std::vector<ScriptValue>& args);
    static ScriptValue getWeek(const std::vector<ScriptValue>& args);
    static ScriptValue getPlayer1(const std::vector<ScriptValue>& args);
    static ScriptValue getPlayer2(const std::vector<ScriptValue>& args);
    static ScriptValue getGfVersion(const std::vector<ScriptValue>& args);

    static ScriptValue keyPressed(const std::vector<ScriptValue>& args);
    static ScriptValue keyJustPressed(const std::vector<ScriptValue>& args);
    static ScriptValue keyReleased(const std::vector<ScriptValue>& args);

    static ScriptValue lerp(const std::vector<ScriptValue>& args);
    static ScriptValue abs(const std::vector<ScriptValue>& args);
    static ScriptValue sin(const std::vector<ScriptValue>& args);
    static ScriptValue cos(const std::vector<ScriptValue>& args);
    static ScriptValue sqrt(const std::vector<ScriptValue>& args);
    static ScriptValue mod(const std::vector<ScriptValue>& args);
};