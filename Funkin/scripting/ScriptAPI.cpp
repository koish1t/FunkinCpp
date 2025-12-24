#include "ScriptAPI.h"
#include "../play/PlayState.h"
#include "../play/song/Conductor.h"
#include "../play/components/HealthBar.h"
#include "../play/notes/NoteHitHandler.h"
#include "../play/character/Character.h"
#include "../game/GameConfig.h"
#include <flixel/FlxG.h>
#include <flixel/FlxSprite.h>
#include <flixel/text/FlxText.h>
#include <flixel/sound/FlxSound.h>
#include <flixel/tweens/FlxTween.h>
#include <flixel/tweens/FlxEase.h>
#include <flixel/util/FlxTimer.h>
#include <flixel/input/FlxKeyboard.h>
#include <iostream>
#include <cmath>
#include <random>

std::map<std::string, ScriptFunction> ScriptAPI::functions;
std::map<std::string, flixel::FlxSprite*> ScriptAPI::sprites;
std::map<std::string, flixel::FlxText*> ScriptAPI::texts;
std::map<std::string, flixel::tweens::FlxTween*> ScriptAPI::tweens;
std::map<std::string, flixel::util::FlxTimer*> ScriptAPI::timers;

int ScriptAPI::getInt(const ScriptValue& val) {
    if (std::holds_alternative<int>(val)) return std::get<int>(val);
    if (std::holds_alternative<float>(val)) return static_cast<int>(std::get<float>(val));
    if (std::holds_alternative<bool>(val)) return std::get<bool>(val) ? 1 : 0;
    return 0;
}

float ScriptAPI::getFloat(const ScriptValue& val) {
    if (std::holds_alternative<float>(val)) return std::get<float>(val);
    if (std::holds_alternative<int>(val)) return static_cast<float>(std::get<int>(val));
    if (std::holds_alternative<bool>(val)) return std::get<bool>(val) ? 1.0f : 0.0f;
    return 0.0f;
}

bool ScriptAPI::getBool(const ScriptValue& val) {
    if (std::holds_alternative<bool>(val)) return std::get<bool>(val);
    if (std::holds_alternative<int>(val)) return std::get<int>(val) != 0;
    if (std::holds_alternative<float>(val)) return std::get<float>(val) != 0.0f;
    return false;
}

std::string ScriptAPI::getString(const ScriptValue& val) {
    if (std::holds_alternative<std::string>(val)) return std::get<std::string>(val);
    if (std::holds_alternative<int>(val)) return std::to_string(std::get<int>(val));
    if (std::holds_alternative<float>(val)) return std::to_string(std::get<float>(val));
    if (std::holds_alternative<bool>(val)) return std::get<bool>(val) ? "true" : "false";
    return "";
}

void ScriptAPI::initialize() {
    if (!functions.empty()) return;

    functions["debugPrint"] = debugPrint;
    functions["print"] = debugPrint;
    functions["trace"] = debugPrint;

    functions["getProperty"] = getProperty;
    functions["setProperty"] = setProperty;

    functions["getRandomInt"] = getRandomInt;
    functions["getRandomFloat"] = getRandomFloat;
    functions["lerp"] = lerp;
    functions["abs"] = abs;
    functions["sin"] = sin;
    functions["cos"] = cos;
    functions["sqrt"] = sqrt;
    functions["mod"] = mod;

    functions["setHealth"] = setHealth;
    functions["getHealth"] = getHealth;
    functions["setScore"] = setScore;
    functions["getScore"] = getScore;
    functions["addScore"] = addScore;
    functions["getMisses"] = getMisses;
    functions["setMisses"] = setMisses;
    functions["getCombo"] = getCombo;
    functions["setCombo"] = setCombo;
    functions["getSicks"] = getSicks;
    functions["getGoods"] = getGoods;
    functions["getBads"] = getBads;
    functions["getShits"] = getShits;

    functions["playCharAnim"] = playCharAnim;
    functions["characterPlayAnim"] = playCharAnim;
    functions["setCharPos"] = setCharPos;
    functions["getCharPos"] = getCharPos;
    functions["getCharX"] = getCharX;
    functions["getCharY"] = getCharY;
    functions["setCharVisible"] = setCharVisible;

    functions["setCameraPos"] = setCameraPos;
    functions["setCameraZoom"] = setCameraZoom;
    functions["getCameraZoom"] = getCameraZoom;
    functions["cameraShake"] = cameraShake;
    functions["cameraFlash"] = cameraFlash;
    functions["cameraFade"] = cameraFade;

    functions["playSound"] = playSound;
    functions["playMusic"] = playMusic;
    functions["stopSound"] = stopSound;
    functions["pauseSound"] = pauseSound;
    functions["resumeSound"] = resumeSound;
    functions["setSoundVolume"] = setSoundVolume;
    functions["getSoundVolume"] = getSoundVolume;
    functions["getSoundTime"] = getSoundTime;

    functions["makeSprite"] = makeSprite;
    functions["makeSpriteGraphic"] = makeSpriteGraphic;
    functions["setSpritePos"] = setSpritePos;
    functions["setSpriteScale"] = setSpriteScale;
    functions["setSpriteVisible"] = setSpriteVisible;
    functions["setSpriteAlpha"] = setSpriteAlpha;
    functions["setSpriteAngle"] = setSpriteAngle;
    functions["removeSprite"] = removeSprite;

    functions["makeText"] = makeText;
    functions["setTextString"] = setTextString;
    functions["setTextSize"] = setTextSize;
    functions["setTextColor"] = setTextColor;
    functions["setTextFont"] = setTextFont;

    functions["doTween"] = doTween;
    functions["cancelTween"] = cancelTween;

    functions["startTimer"] = startTimer;
    functions["cancelTimer"] = cancelTimer;

    functions["getSongPosition"] = getSongPosition;
    functions["getSongLength"] = getSongLength;
    functions["getCurrentBeat"] = getCurrentBeat;
    functions["getCurrentStep"] = getCurrentStep;
    functions["getBPM"] = getBPM;
    functions["getCrochet"] = getCrochet;
    functions["getStepCrochet"] = getStepCrochet;

    functions["getStoryMode"] = getStoryMode;
    functions["isStoryMode"] = getStoryMode;
    functions["getDifficulty"] = getDifficulty;
    functions["getSongName"] = getSongName;
    functions["getWeek"] = getWeek;
    functions["getPlayer1"] = getPlayer1;
    functions["getPlayer2"] = getPlayer2;
    functions["getGfVersion"] = getGfVersion;

    functions["keyPressed"] = keyPressed;
    functions["keyJustPressed"] = keyJustPressed;
    functions["keyReleased"] = keyReleased;
}

void ScriptAPI::cleanup() {
    for (auto& pair : sprites) {
        if (pair.second) delete pair.second;
    }
    sprites.clear();

    for (auto& pair : texts) {
        if (pair.second) delete pair.second;
    }
    texts.clear();

    tweens.clear();
    timers.clear();
}

bool ScriptAPI::hasFunction(const std::string& funcName) {
    return functions.find(funcName) != functions.end();
}

ScriptValue ScriptAPI::callFunction(const std::string& funcName, const std::vector<ScriptValue>& args) {
    auto it = functions.find(funcName);
    if (it != functions.end()) {
        return it->second(args);
    }
    return 0;
}

ScriptValue ScriptAPI::debugPrint(const std::vector<ScriptValue>& args) {
    for (size_t i = 0; i < args.size(); i++) {
        std::cout << getString(args[i]);
        if (i < args.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
    return 0;
}

ScriptValue ScriptAPI::getProperty(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0;
    std::string objName = getString(args[0]);

    auto spriteIt = sprites.find(objName);
    if (spriteIt != sprites.end() && spriteIt->second && args.size() > 1) {
        std::string prop = getString(args[1]);
        flixel::FlxSprite* sprite = spriteIt->second;

        if (prop == "x") return sprite->x;
        if (prop == "y") return sprite->y;
        if (prop == "alpha") return sprite->alpha;
        if (prop == "angle") return sprite->angle;
        if (prop == "visible") return sprite->visible;
    }

    auto textIt = texts.find(objName);
    if (textIt != texts.end() && textIt->second && args.size() > 1) {
        std::string prop = getString(args[1]);
        flixel::FlxText* text = textIt->second;

        if (prop == "x") return text->x;
        if (prop == "y") return text->y;
        if (prop == "alpha") return text->alpha;
        if (prop == "visible") return text->visible;
    }

    return 0;
}

ScriptValue ScriptAPI::setProperty(const std::vector<ScriptValue>& args) {
    if (args.size() < 3) return 0;
    std::string objName = getString(args[0]);
    std::string prop = getString(args[1]);

    auto spriteIt = sprites.find(objName);
    if (spriteIt != sprites.end() && spriteIt->second) {
        flixel::FlxSprite* sprite = spriteIt->second;

        if (prop == "x") sprite->x = getFloat(args[2]);
        else if (prop == "y") sprite->y = getFloat(args[2]);
        else if (prop == "alpha") sprite->alpha = getFloat(args[2]);
        else if (prop == "angle") sprite->angle = getFloat(args[2]);
        else if (prop == "visible") sprite->visible = getBool(args[2]);

        return 0;
    }

    auto textIt = texts.find(objName);
    if (textIt != texts.end() && textIt->second) {
        flixel::FlxText* text = textIt->second;

        if (prop == "x") text->x = getFloat(args[2]);
        else if (prop == "y") text->y = getFloat(args[2]);
        else if (prop == "alpha") text->alpha = getFloat(args[2]);
        else if (prop == "visible") text->visible = getBool(args[2]);

        return 0;
    }

    return 0;
}

ScriptValue ScriptAPI::setHealth(const std::vector<ScriptValue>& args) {
    if (args.empty() || !PlayState::instance) return 0;

    HealthBar* healthBar = PlayState::instance->getHealthBar();
    if (healthBar) {
        float health = getFloat(args[0]);
        healthBar->setHealth(health);
    }

    return 0;
}

ScriptValue ScriptAPI::getHealth(const std::vector<ScriptValue>& args) {
    if (!PlayState::instance) return 0.0f;

    HealthBar* healthBar = PlayState::instance->getHealthBar();
    if (healthBar) {
        return healthBar->getHealth();
    }

    return 1.0f;
}

ScriptValue ScriptAPI::setScore(const std::vector<ScriptValue>& args) {
    if (args.empty() || !PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        int score = getInt(args[0]);
        noteHitHandler->setScore(score);
        noteHitHandler->updateScore();
    }

    return 0;
}

ScriptValue ScriptAPI::getScore(const std::vector<ScriptValue>& args) {
    if (!PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        return noteHitHandler->getScore();
    }

    return 0;
}

ScriptValue ScriptAPI::addScore(const std::vector<ScriptValue>& args) {
    if (args.empty() || !PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        int amount = getInt(args[0]);
        noteHitHandler->incrementScore(amount);
        noteHitHandler->updateScore();
    }

    return 0;
}

ScriptValue ScriptAPI::getMisses(const std::vector<ScriptValue>& args) {
    if (!PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        return noteHitHandler->getMisses();
    }

    return 0;
}

ScriptValue ScriptAPI::setMisses(const std::vector<ScriptValue>& args) {
    if (args.empty() || !PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        int misses = getInt(args[0]);
        noteHitHandler->setMisses(misses);
        noteHitHandler->updateScore();
    }

    return 0;
}

ScriptValue ScriptAPI::getCombo(const std::vector<ScriptValue>& args) {
    if (!PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        return noteHitHandler->getCombo();
    }

    return 0;
}

ScriptValue ScriptAPI::setCombo(const std::vector<ScriptValue>& args) {
    if (args.empty() || !PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        int combo = getInt(args[0]);
        noteHitHandler->setCombo(combo);
    }

    return 0;
}

ScriptValue ScriptAPI::getSicks(const std::vector<ScriptValue>& args) {
    if (!PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        return noteHitHandler->getSicks();
    }

    return 0;
}

ScriptValue ScriptAPI::getGoods(const std::vector<ScriptValue>& args) {
    if (!PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        return noteHitHandler->getGoods();
    }

    return 0;
}

ScriptValue ScriptAPI::getBads(const std::vector<ScriptValue>& args) {
    if (!PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        return noteHitHandler->getBads();
    }

    return 0;
}

ScriptValue ScriptAPI::getShits(const std::vector<ScriptValue>& args) {
    if (!PlayState::instance) return 0;

    NoteHitHandler* noteHitHandler = PlayState::instance->getNoteHitHandler();
    if (noteHitHandler) {
        return noteHitHandler->getShits();
    }

    return 0;
}

ScriptValue ScriptAPI::playCharAnim(const std::vector<ScriptValue>& args) {
    if (args.size() < 2 || !PlayState::instance) return 0;
    std::string charName = getString(args[0]);
    std::string animName = getString(args[1]);
    bool forced = args.size() > 2 ? getBool(args[2]) : false;

    Character* character = nullptr;
    if (charName == "bf" || charName == "boyfriend") {
        character = PlayState::instance->getBoyfriend();
    } else if (charName == "dad" || charName == "opponent") {
        character = PlayState::instance->getDad();
    } else if (charName == "gf" || charName == "girlfriend") {
        character = PlayState::instance->getGf();
    }

    if (character) {
        character->playAnim(animName, forced);
    }

    return 0;
}

ScriptValue ScriptAPI::setCharPos(const std::vector<ScriptValue>& args) {
    if (args.size() < 3 || !PlayState::instance) return 0;
    std::string charName = getString(args[0]);
    float x = getFloat(args[1]);
    float y = getFloat(args[2]);

    Character* character = nullptr;
    if (charName == "bf" || charName == "boyfriend") {
        character = PlayState::instance->getBoyfriend();
    } else if (charName == "dad" || charName == "opponent") {
        character = PlayState::instance->getDad();
    } else if (charName == "gf" || charName == "girlfriend") {
        character = PlayState::instance->getGf();
    }

    if (character) {
        character->setPosition(x, y);
    }

    return 0;
}

ScriptValue ScriptAPI::getCharPos(const std::vector<ScriptValue>& args) {
    if (args.size() < 2 || !PlayState::instance) return 0;
    std::string charName = getString(args[0]);
    std::string axis = getString(args[1]);

    Character* character = nullptr;
    if (charName == "bf" || charName == "boyfriend") {
        character = PlayState::instance->getBoyfriend();
    } else if (charName == "dad" || charName == "opponent") {
        character = PlayState::instance->getDad();
    } else if (charName == "gf" || charName == "girlfriend") {
        character = PlayState::instance->getGf();
    }

    if (character) {
        if (axis == "x") return character->x;
        if (axis == "y") return character->y;
    }

    return 0.0f;
}

ScriptValue ScriptAPI::getCharX(const std::vector<ScriptValue>& args) {
    if (args.empty() || !PlayState::instance) return 0.0f;
    std::string charName = getString(args[0]);

    Character* character = nullptr;
    if (charName == "bf" || charName == "boyfriend") {
        character = PlayState::instance->getBoyfriend();
    } else if (charName == "dad" || charName == "opponent") {
        character = PlayState::instance->getDad();
    } else if (charName == "gf" || charName == "girlfriend") {
        character = PlayState::instance->getGf();
    }

    return character ? character->x : 0.0f;
}

ScriptValue ScriptAPI::getCharY(const std::vector<ScriptValue>& args) {
    if (args.empty() || !PlayState::instance) return 0.0f;
    std::string charName = getString(args[0]);

    Character* character = nullptr;
    if (charName == "bf" || charName == "boyfriend") {
        character = PlayState::instance->getBoyfriend();
    } else if (charName == "dad" || charName == "opponent") {
        character = PlayState::instance->getDad();
    } else if (charName == "gf" || charName == "girlfriend") {
        character = PlayState::instance->getGf();
    }

    return character ? character->y : 0.0f;
}

ScriptValue ScriptAPI::setCharVisible(const std::vector<ScriptValue>& args) {
    if (args.size() < 2 || !PlayState::instance) return 0;
    std::string charName = getString(args[0]);
    bool visible = getBool(args[1]);

    Character* character = nullptr;
    if (charName == "bf" || charName == "boyfriend") {
        character = PlayState::instance->getBoyfriend();
    } else if (charName == "dad" || charName == "opponent") {
        character = PlayState::instance->getDad();
    } else if (charName == "gf" || charName == "girlfriend") {
        character = PlayState::instance->getGf();
    }

    if (character) {
        character->visible = visible;
    }

    return 0;
}

ScriptValue ScriptAPI::setCameraPos(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    float x = getFloat(args[0]);
    float y = getFloat(args[1]);
    std::string camName = args.size() > 2 ? getString(args[2]) : "game";

    flixel::FlxCamera* targetCam = nullptr;
    if (PlayState::instance) {
        if (camName == "hud") {
            targetCam = PlayState::instance->getCamHUD();
        } else {
            targetCam = PlayState::instance->getCamGame();
        }
    }

    if (targetCam) {
        targetCam->scroll.x = x;
        targetCam->scroll.y = y;
    }

    return 0;
}

ScriptValue ScriptAPI::setCameraZoom(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0;
    float zoom = getFloat(args[0]);
    std::string camName = args.size() > 1 ? getString(args[1]) : "game";

    flixel::FlxCamera* targetCam = nullptr;
    if (PlayState::instance) {
        if (camName == "hud") {
            targetCam = PlayState::instance->getCamHUD();
        } else {
            targetCam = PlayState::instance->getCamGame();
        }
    }

    if (targetCam) {
        targetCam->zoom = zoom;
    }

    return 0;
}

ScriptValue ScriptAPI::getCameraZoom(const std::vector<ScriptValue>& args) {
    std::string camName = args.empty() ? "game" : getString(args[0]);

    flixel::FlxCamera* targetCam = nullptr;
    if (PlayState::instance) {
        if (camName == "hud") {
            targetCam = PlayState::instance->getCamHUD();
        } else {
            targetCam = PlayState::instance->getCamGame();
        }
    }

    if (targetCam) {
        return targetCam->zoom;
    }
    return 1.0f;
}

ScriptValue ScriptAPI::cameraShake(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    float intensity = getFloat(args[0]);
    float duration = getFloat(args[1]);
    std::string camName = args.size() > 2 ? getString(args[2]) : "game";

    flixel::FlxCamera* targetCam = nullptr;
    if (PlayState::instance) {
        if (camName == "hud") {
            targetCam = PlayState::instance->getCamHUD();
        } else {
            targetCam = PlayState::instance->getCamGame();
        }
    }

    if (targetCam) {
        targetCam->shake(intensity, duration);
    }

    return 0;
}

ScriptValue ScriptAPI::cameraFlash(const std::vector<ScriptValue>& args) {
    return 0;
}

ScriptValue ScriptAPI::cameraFade(const std::vector<ScriptValue>& args) {
    return 0;
}

ScriptValue ScriptAPI::playSound(const std::vector<ScriptValue>& args) {
    if (args.empty()) {
        return 0;
    }
    std::string soundPath = getString(args[0]);
    float volume = args.size() > 1 ? getFloat(args[1]) : 1.0f;

    flixel::FlxG::sound.playAsChunk(soundPath.c_str(), volume);
    return 0;
}

ScriptValue ScriptAPI::playMusic(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0;
    std::string musicPath = getString(args[0]);
    bool looped = args.size() > 1 ? getBool(args[1]) : true;

    flixel::FlxG::sound.playMusic(musicPath.c_str(), looped);

    return 0;
}

ScriptValue ScriptAPI::stopSound(const std::vector<ScriptValue>& args) {
    if (flixel::FlxG::sound.music) {
        flixel::FlxG::sound.music->stop();
    }
    return 0;
}

ScriptValue ScriptAPI::pauseSound(const std::vector<ScriptValue>& args) {
    if (flixel::FlxG::sound.music) {
        flixel::FlxG::sound.music->pause();
    }
    return 0;
}

ScriptValue ScriptAPI::resumeSound(const std::vector<ScriptValue>& args) {
    if (flixel::FlxG::sound.music) {
        flixel::FlxG::sound.music->resume();
    }
    return 0;
}

ScriptValue ScriptAPI::setSoundVolume(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0;
    float volume = getFloat(args[0]);

    if (flixel::FlxG::sound.music) {
        flixel::FlxG::sound.music->setVolume(volume);
    }

    return 0;
}

ScriptValue ScriptAPI::getSoundVolume(const std::vector<ScriptValue>& args) {
    if (flixel::FlxG::sound.music) {
        return flixel::FlxG::sound.music->get_volume();
    }
    return 0.0f;
}

ScriptValue ScriptAPI::getSoundTime(const std::vector<ScriptValue>& args) {
    if (flixel::FlxG::sound.music) {
        return flixel::FlxG::sound.music->get_time();
    }
    return 0.0f;
}

ScriptValue ScriptAPI::makeSprite(const std::vector<ScriptValue>& args) {
    if (args.size() < 3) return 0;
    std::string tag = getString(args[0]);
    std::string imagePath = getString(args[1]);
    float x = getFloat(args[2]);
    float y = args.size() > 3 ? getFloat(args[3]) : 0.0f;

    flixel::FlxSprite* sprite = new flixel::FlxSprite(x, y);
    sprite->loadGraphic(imagePath.c_str());
    sprite->camera = flixel::FlxG::camera;

    if (sprites.find(tag) != sprites.end()) {
        delete sprites[tag];
    }
    sprites[tag] = sprite;

    return 0;
}

ScriptValue ScriptAPI::makeSpriteGraphic(const std::vector<ScriptValue>& args) {
    if (args.size() < 3) return 0;
    std::string tag = getString(args[0]);
    int width = getInt(args[1]);
    int height = getInt(args[2]);
    uint32_t color = args.size() > 3 ? static_cast<uint32_t>(getInt(args[3])) : 0xFFFFFFFF;

    flixel::FlxSprite* sprite = new flixel::FlxSprite(0, 0);
    SDL_Color sdlColor = {
        static_cast<Uint8>((color >> 16) & 0xFF),
        static_cast<Uint8>((color >> 8) & 0xFF),
        static_cast<Uint8>(color & 0xFF),
        static_cast<Uint8>((color >> 24) & 0xFF)
    };
    sprite->makeGraphic(width, height, sdlColor);
    sprite->camera = flixel::FlxG::camera;

    if (sprites.find(tag) != sprites.end()) {
        delete sprites[tag];
    }
    sprites[tag] = sprite;

    return 0;
}

ScriptValue ScriptAPI::setSpritePos(const std::vector<ScriptValue>& args) {
    if (args.size() < 3) return 0;
    std::string tag = getString(args[0]);

    auto it = sprites.find(tag);
    if (it != sprites.end() && it->second) {
        it->second->x = getFloat(args[1]);
        it->second->y = getFloat(args[2]);
    }

    return 0;
}

ScriptValue ScriptAPI::setSpriteScale(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    std::string tag = getString(args[0]);
    float scale = getFloat(args[1]);

    auto it = sprites.find(tag);
    if (it != sprites.end() && it->second) {
        it->second->scale.x = scale;
        it->second->scale.y = scale;
    }

    return 0;
}

ScriptValue ScriptAPI::setSpriteVisible(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    std::string tag = getString(args[0]);
    bool visible = getBool(args[1]);

    auto it = sprites.find(tag);
    if (it != sprites.end() && it->second) {
        it->second->visible = visible;
    }

    return 0;
}

ScriptValue ScriptAPI::setSpriteAlpha(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    std::string tag = getString(args[0]);
    float alpha = getFloat(args[1]);

    auto it = sprites.find(tag);
    if (it != sprites.end() && it->second) {
        it->second->alpha = alpha;
    }

    return 0;
}

ScriptValue ScriptAPI::setSpriteAngle(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    std::string tag = getString(args[0]);
    float angle = getFloat(args[1]);

    auto it = sprites.find(tag);
    if (it != sprites.end() && it->second) {
        it->second->angle = angle;
    }

    return 0;
}

ScriptValue ScriptAPI::removeSprite(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0;
    std::string tag = getString(args[0]);

    auto it = sprites.find(tag);
    if (it != sprites.end()) {
        if (it->second) delete it->second;
        sprites.erase(it);
    }

    return 0;
}

ScriptValue ScriptAPI::makeText(const std::vector<ScriptValue>& args) {
    if (args.size() < 4) return 0;
    std::string tag = getString(args[0]);
    float x = getFloat(args[1]);
    float y = getFloat(args[2]);
    std::string text = getString(args[3]);
    int size = args.size() > 4 ? getInt(args[4]) : 16;

    flixel::FlxText* textObj = new flixel::FlxText(x, y, 0, text.c_str(), size);

    if (PlayState::instance) {
        textObj->camera = PlayState::instance->getCamHUD();
    } else {
        textObj->camera = flixel::FlxG::camera;
    }

    if (texts.find(tag) != texts.end()) {
        delete texts[tag];
    }
    texts[tag] = textObj;

    return 0;
}

ScriptValue ScriptAPI::setTextString(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    std::string tag = getString(args[0]);
    std::string text = getString(args[1]);

    auto it = texts.find(tag);
    if (it != texts.end() && it->second) {
        it->second->setText(text);
    }

    return 0;
}

ScriptValue ScriptAPI::setTextSize(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    std::string tag = getString(args[0]);
    int size = getInt(args[1]);

    auto it = texts.find(tag);
    if (it != texts.end() && it->second) {
        it->second->setSize(size);
    }

    return 0;
}

ScriptValue ScriptAPI::setTextColor(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    std::string tag = getString(args[0]);
    uint32_t color = static_cast<uint32_t>(getInt(args[1]));

    auto it = texts.find(tag);
    if (it != texts.end() && it->second) {
        it->second->setColor(color);
    }

    return 0;
}

ScriptValue ScriptAPI::setTextFont(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    std::string tag = getString(args[0]);
    std::string font = getString(args[1]);

    auto it = texts.find(tag);
    if (it != texts.end() && it->second) {
        it->second->setFont(font.c_str());
    }

    return 0;
}

ScriptValue ScriptAPI::doTween(const std::vector<ScriptValue>& args) {
    if (args.size() < 5) return 0;
    std::string tag = getString(args[0]);
    std::string objName = getString(args[1]);
    std::string property = getString(args[2]);
    float targetValue = getFloat(args[3]);
    float duration = getFloat(args[4]);
    std::string easeStr = args.size() > 5 ? getString(args[5]) : "linear";

    flixel::FlxSprite* sprite = nullptr;
    auto spriteIt = sprites.find(objName);
    if (spriteIt != sprites.end()) {
        sprite = spriteIt->second;
    }

    auto textIt = texts.find(objName);
    flixel::FlxText* text = nullptr;
    if (textIt != texts.end()) {
        text = textIt->second;
    }

    if (!sprite && !text) return 0;

    std::map<std::string, float> props;
    props[property] = targetValue;

    flixel::tweens::EaseFunction ease = flixel::tweens::FlxEase::linear;
    if (easeStr == "quadIn") ease = flixel::tweens::FlxEase::quadIn;
    else if (easeStr == "quadOut") ease = flixel::tweens::FlxEase::quadOut;
    else if (easeStr == "quadInOut") ease = flixel::tweens::FlxEase::quadInOut;
    else if (easeStr == "cubeIn") ease = flixel::tweens::FlxEase::cubeIn;
    else if (easeStr == "cubeOut") ease = flixel::tweens::FlxEase::cubeOut;
    else if (easeStr == "cubeInOut") ease = flixel::tweens::FlxEase::cubeInOut;

    flixel::tweens::FlxTween* tween = nullptr;
    if (sprite) {
        tween = flixel::tweens::tween(static_cast<void*>(sprite), props, duration, ease);
    } else if (text) {
        tween = flixel::tweens::tween(static_cast<void*>(text), props, duration, ease);
    }

    if (tween) {
        tweens[tag] = tween;
    }

    return 0;
}

ScriptValue ScriptAPI::cancelTween(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0;
    std::string tag = getString(args[0]);

    auto it = tweens.find(tag);
    if (it != tweens.end()) {
        if (it->second) {
            it->second->cancel();
        }
        tweens.erase(it);
    }

    return 0;
}

ScriptValue ScriptAPI::startTimer(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    std::string tag = getString(args[0]);
    float duration = getFloat(args[1]);

    flixel::util::FlxTimer* timer = new flixel::util::FlxTimer();
    timer->start(duration, [tag](flixel::util::FlxTimer* t) {
        std::cout << "Timer " << tag << " completed" << std::endl;
    });

    timers[tag] = timer;

    return 0;
}

ScriptValue ScriptAPI::cancelTimer(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0;
    std::string tag = getString(args[0]);

    auto it = timers.find(tag);
    if (it != timers.end()) {
        if (it->second) {
            it->second->cancel();
            delete it->second;
        }
        timers.erase(it);
    }

    return 0;
}

ScriptValue ScriptAPI::getSongPosition(const std::vector<ScriptValue>& args) {
    return Conductor::songPosition;
}

ScriptValue ScriptAPI::getSongLength(const std::vector<ScriptValue>& args) {
    if (PlayState::inst) {
        return PlayState::inst->get_length();
    }
    return 0.0f;
}

ScriptValue ScriptAPI::getCurrentBeat(const std::vector<ScriptValue>& args) {
    if (PlayState::instance) {
        return PlayState::instance->getCurBeat();
    }
    return 0;
}

ScriptValue ScriptAPI::getCurrentStep(const std::vector<ScriptValue>& args) {
    if (PlayState::instance) {
        return PlayState::instance->getCurStep();
    }
    return 0;
}

ScriptValue ScriptAPI::getBPM(const std::vector<ScriptValue>& args) {
    return static_cast<int>(Conductor::bpm);
}

ScriptValue ScriptAPI::getCrochet(const std::vector<ScriptValue>& args) {
    return Conductor::crochet;
}

ScriptValue ScriptAPI::getStepCrochet(const std::vector<ScriptValue>& args) {
    return Conductor::stepCrochet;
}

ScriptValue ScriptAPI::getStoryMode(const std::vector<ScriptValue>& args) {
    return PlayState::isStoryMode;
}

ScriptValue ScriptAPI::getDifficulty(const std::vector<ScriptValue>& args) {
    return PlayState::storyDifficulty;
}

ScriptValue ScriptAPI::getSongName(const std::vector<ScriptValue>& args) {
    return PlayState::SONG.song;
}

ScriptValue ScriptAPI::getWeek(const std::vector<ScriptValue>& args) {
    return PlayState::storyWeek;
}

ScriptValue ScriptAPI::getPlayer1(const std::vector<ScriptValue>& args) {
    return PlayState::SONG.player1;
}

ScriptValue ScriptAPI::getPlayer2(const std::vector<ScriptValue>& args) {
    return PlayState::SONG.player2;
}

ScriptValue ScriptAPI::getGfVersion(const std::vector<ScriptValue>& args) {
    return PlayState::SONG.gfVersion;
}

ScriptValue ScriptAPI::keyPressed(const std::vector<ScriptValue>& args) {
    if (args.empty()) return false;
    std::string keyName = getString(args[0]);

    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
    if (keyName == "space") scancode = SDL_SCANCODE_SPACE;
    else if (keyName == "left") scancode = SDL_SCANCODE_LEFT;
    else if (keyName == "right") scancode = SDL_SCANCODE_RIGHT;
    else if (keyName == "up") scancode = SDL_SCANCODE_UP;
    else if (keyName == "down") scancode = SDL_SCANCODE_DOWN;
    else if (keyName == "enter") scancode = SDL_SCANCODE_RETURN;
    else if (keyName == "escape") scancode = SDL_SCANCODE_ESCAPE;
    else if (keyName.length() == 1) {
        char c = keyName[0];
        if (c >= 'a' && c <= 'z') {
            scancode = static_cast<SDL_Scancode>(SDL_SCANCODE_A + (c - 'a'));
        }
    }

    if (scancode != SDL_SCANCODE_UNKNOWN) {
        auto it = flixel::FlxG::keys.keys.find(scancode);
        if (it != flixel::FlxG::keys.keys.end()) {
            return it->second.pressed();
        }
    }

    return false;
}

ScriptValue ScriptAPI::keyJustPressed(const std::vector<ScriptValue>& args) {
    if (args.empty()) return false;
    std::string keyName = getString(args[0]);

    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
    if (keyName == "space") scancode = SDL_SCANCODE_SPACE;
    else if (keyName == "left") scancode = SDL_SCANCODE_LEFT;
    else if (keyName == "right") scancode = SDL_SCANCODE_RIGHT;
    else if (keyName == "up") scancode = SDL_SCANCODE_UP;
    else if (keyName == "down") scancode = SDL_SCANCODE_DOWN;
    else if (keyName == "enter") scancode = SDL_SCANCODE_RETURN;
    else if (keyName == "escape") scancode = SDL_SCANCODE_ESCAPE;
    else if (keyName.length() == 1) {
        char c = keyName[0];
        if (c >= 'a' && c <= 'z') {
            scancode = static_cast<SDL_Scancode>(SDL_SCANCODE_A + (c - 'a'));
        }
    }

    if (scancode != SDL_SCANCODE_UNKNOWN) {
        auto it = flixel::FlxG::keys.keys.find(scancode);
        if (it != flixel::FlxG::keys.keys.end()) {
            return it->second.justPressed();
        }
    }

    return false;
}

ScriptValue ScriptAPI::keyReleased(const std::vector<ScriptValue>& args) {
    if (args.empty()) return false;
    std::string keyName = getString(args[0]);

    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
    if (keyName == "space") scancode = SDL_SCANCODE_SPACE;
    else if (keyName == "left") scancode = SDL_SCANCODE_LEFT;
    else if (keyName == "right") scancode = SDL_SCANCODE_RIGHT;
    else if (keyName == "up") scancode = SDL_SCANCODE_UP;
    else if (keyName == "down") scancode = SDL_SCANCODE_DOWN;
    else if (keyName == "enter") scancode = SDL_SCANCODE_RETURN;
    else if (keyName == "escape") scancode = SDL_SCANCODE_ESCAPE;
    else if (keyName.length() == 1) {
        char c = keyName[0];
        if (c >= 'a' && c <= 'z') {
            scancode = static_cast<SDL_Scancode>(SDL_SCANCODE_A + (c - 'a'));
        }
    }

    if (scancode != SDL_SCANCODE_UNKNOWN) {
        auto it = flixel::FlxG::keys.keys.find(scancode);
        if (it != flixel::FlxG::keys.keys.end()) {
            return it->second.justReleased();
        }
    }

    return false;
}

ScriptValue ScriptAPI::getRandomInt(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    int min = getInt(args[0]);
    int max = getInt(args[1]);

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    return dis(gen);
}

ScriptValue ScriptAPI::getRandomFloat(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0.0f;
    float min = getFloat(args[0]);
    float max = getFloat(args[1]);

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);

    return static_cast<float>(dis(gen));
}

ScriptValue ScriptAPI::lerp(const std::vector<ScriptValue>& args) {
    if (args.size() < 3) return 0.0f;
    float a = getFloat(args[0]);
    float b = getFloat(args[1]);
    float t = getFloat(args[2]);

    return a + (b - a) * t;
}

ScriptValue ScriptAPI::abs(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0.0f;
    return std::abs(getFloat(args[0]));
}

ScriptValue ScriptAPI::sin(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0.0f;
    return std::sin(getFloat(args[0]));
}

ScriptValue ScriptAPI::cos(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0.0f;
    return std::cos(getFloat(args[0]));
}

ScriptValue ScriptAPI::sqrt(const std::vector<ScriptValue>& args) {
    if (args.empty()) return 0.0f;
    return std::sqrt(getFloat(args[0]));
}

ScriptValue ScriptAPI::mod(const std::vector<ScriptValue>& args) {
    if (args.size() < 2) return 0;
    int a = getInt(args[0]);
    int b = getInt(args[1]);
    if (b == 0) return 0;
    return a % b;
}