#include "PlayState.h"
#include <iostream>
#include <algorithm>
#include "../game/Song.h"
#include "../game/Conductor.h"
#include <fstream>
#include <map>
#include "../../../external/nlohmann/json.hpp"
#include <flixel/FlxG.h>

PlayState* PlayState::instance = nullptr;
SwagSong PlayState::SONG;
flixel::FlxSound* PlayState::inst = nullptr;

const char* PlayState::NOTE_STYLES[] = {"purple", "blue", "green", "red"};
const char* PlayState::NOTE_DIRS[] = {"LEFT", "DOWN", "UP", "RIGHT"};

PlayState::PlayState() {
    instance = this;
    inst = nullptr;
    vocals = nullptr;
    camGame = nullptr;
    camHUD = nullptr;
    Note::loadAssets();
    scoreText = new flixel::FlxText(0, 0, 0, "");
    scoreText->setFont("assets/fonts/monsterrat.ttf");
    scoreText->setSize(32);
    
    int windowWidth = flixel::FlxG::width;
    int windowHeight = flixel::FlxG::height;
    scoreText->setPosition(static_cast<float>(windowWidth / 2 - 100), static_cast<float>(windowHeight - 50));
    updateScoreText();

    loadKeybinds();
    
    startingSong = false;
    startedCountdown = false;
    pauseCooldown = 0.0f;
    musicStartTicks = 0;
    score = 0;
    misses = 0;
    combo = 0;
}

PlayState::~PlayState() {
    if (vocals != nullptr) {
        delete vocals;
        vocals = nullptr;
    }
    if (inst != nullptr) {
        delete inst;
        inst = nullptr;
    }
    
    if (camGame != nullptr) {
        delete camGame;
        camGame = nullptr;
    }
    if (camHUD != nullptr) {
        delete camHUD;
        camHUD = nullptr;
    }
    
    for (auto arrow : strumLineNotes) {
        delete arrow;
    }
    strumLineNotes.clear();
    
    for (auto note : notes) {
        delete note;
    }
    notes.clear();
    
    delete scoreText;
    Note::unloadAssets();
    destroy();
}

void PlayState::loadSongConfig() {
    std::ifstream configFile("assets/data/config.json");
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config.json" << std::endl;
        generateSong("fnf2");
        return;
    }

    try {
        nlohmann::json config;
        configFile >> config;

        if (config.contains("songConfig")) {
            auto songConfig = config["songConfig"];
            std::string songName = songConfig["songName"].get<std::string>();
            std::string difficulty = songConfig["difficulty"].get<std::string>();
            
            std::string fullSongName = difficulty.empty() ? songName : songName + "-" + difficulty;
            generateSong(fullSongName);
        } else {
            std::cerr << "No songConfig found in config.json, using default" << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Failed to parse song config: " << ex.what() << std::endl;
    }
}

void PlayState::create() {
    Conductor::songPosition = 0;
    startingSong = true;
    startedCountdown = false;
    
    camGame = new flixel::FlxCamera(0.0f, 0.0f, 0, 0, 1.0f);
    camHUD = new flixel::FlxCamera(0.0f, 0.0f, 0, 0, 1.0f);
    
    setupHUDCamera();
    
    loadSongConfig();
    
    if (!SONG.validScore) {
        std::cout << "Error: Failed to load song data!" << std::endl;
        return;
    }
    
    startCountdown();
    generateNotes();
}

void PlayState::update(float elapsed) {
    FunkinState::update(elapsed);
    
    static bool wasPaused = false;
    if (wasPaused && !subState) {
        if (musicStartTicks > 0) {
            musicStartTicks = SDL_GetTicks() - static_cast<Uint32>(Conductor::songPosition);
        }
        
        if (inst && inst->paused) {
            inst->resume();
        }
        if (vocals && vocals->paused) {
            vocals->resume();
        }
    }
    wasPaused = (subState != nullptr);

    if (!subState) {
        handleInput();
        handleOpponentNoteHit(elapsed);
        updateArrowAnimations();

        for (auto arrow : strumLineNotes) {
            if (arrow) {
                arrow->update(elapsed);
            }
        }

        if (!startingSong && musicStartTicks > 0) {
            Conductor::songPosition = static_cast<float>(SDL_GetTicks() - musicStartTicks);
        } else {
            Conductor::songPosition += elapsed * 1000.0f;
        }

        while (!unspawnNotes.empty()) {
            Note* nextNote = unspawnNotes[0];
            if (nextNote->strumTime - Conductor::songPosition > 1500) {
                break;
            }
            
            notes.push_back(nextNote);
            unspawnNotes.erase(unspawnNotes.begin());
        }

        for (auto it = notes.begin(); it != notes.end();) {
            Note* note = *it;
            if (note) {
                note->update(elapsed);

                if (note->mustPress && note->tooLate && !note->wasGoodHit) {
                    noteMiss(note->noteData);
                    note->kill = true;
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

        if (startingSong && startedCountdown) {
            if (Conductor::songPosition >= 0) {
                startSong();
            }
        }

        if (pauseCooldown > 0) {
            pauseCooldown -= elapsed;
        }

        if (flixel::FlxG::keys.keys[SDL_SCANCODE_RETURN].justPressed()) {
            if (!subState) {
                if (pauseCooldown <= 0) {
                    if (inst) {
                        inst->pause();
                    }
                    if (vocals) {
                        vocals->pause();
                    }
                    
                    if (musicStartTicks > 0) {
                        musicStartTicks = SDL_GetTicks() - static_cast<Uint32>(Conductor::songPosition);
                    }
                    
                    PauseSubState* pauseSubState = new PauseSubState();
                    openSubState(pauseSubState);
                    std::cout << "Pause SubState opened" << std::endl;
                    pauseCooldown = 0.5f;
                }
            } else {
                if (musicStartTicks > 0) {
                    musicStartTicks = SDL_GetTicks() - static_cast<Uint32>(Conductor::songPosition);
                }
                
                if (inst) {
                    inst->resume();
                }
                if (vocals) {
                    vocals->resume();
                }
                
                closeSubState();
                std::cout << "Pause SubState closed" << std::endl;
            }
        }
    }
}

void PlayState::handleInput() {
    for (size_t i = 0; i < 4; i++) {
        size_t arrowIndex = i + 4;
        if (arrowIndex < strumLineNotes.size() && strumLineNotes[arrowIndex]) {
            if (isKeyJustPressed(static_cast<int>(i)) || isNXButtonJustPressed(static_cast<int>(i))) {
                if (strumLineNotes[arrowIndex]->animation) {
                    strumLineNotes[arrowIndex]->animation->play("pressed");
                }
                
                bool noteHit = false;
                for (auto note : notes) {
                    if (note && note->mustPress && !note->wasGoodHit && note->noteData == static_cast<int>(i) && note->canBeHit) {
                        goodNoteHit(note);
                        noteHit = true;
                        break;
                    }
                }
                
                if (!noteHit) {
                    if (GameConfig::getInstance()->isGhostTapping()) {
                    } else {
                        noteMiss(static_cast<int>(i));
                    }
                }
            }
            else if (isKeyJustReleased(static_cast<int>(i)) || isNXButtonJustReleased(static_cast<int>(i))) {
                if (strumLineNotes[arrowIndex]->animation) {
                    strumLineNotes[arrowIndex]->animation->play("static");
                }
            }
        }
    }
}

void PlayState::updateArrowAnimations() {
    for (size_t i = 4; i < strumLineNotes.size(); i++) {
        auto arrow = strumLineNotes[i];
        if (arrow && arrow->animation) {
            int keyIndex = (i - 4) % 4;
            if (arrow->animation->current == "pressed" && !isKeyPressed(keyIndex)) {
                arrow->animation->play("static");
            }
        }
    }
    
    for (auto arrow : strumLineNotes) {
        if (arrow && arrow->frames && arrow->animation) {
            int frameIdx = arrow->animation->getCurrentFrame();
            if (frameIdx >= 0 && frameIdx < static_cast<int>(arrow->frames->frames.size())) {
                const auto& frame = arrow->frames->frames[frameIdx];
                arrow->frameWidth = frame.sourceSize.w;
                arrow->frameHeight = frame.sourceSize.h;
            }
            
            if (arrow->animation->current == "confirm") {
                arrow->centerOffsets();
                arrow->offsetX -= 13.0f;
                arrow->offsetY -= 13.0f;
            } else {
                arrow->centerOffsets();
            }
        }
    }
}

void PlayState::generateSong(std::string dataPath) {
    try {
        std::string songName = dataPath;
        std::string folder = dataPath;
        std::string baseSongName = dataPath;
        
        if (folder.length() >= 5 && folder.substr(folder.length() - 5) == "-easy" ||
            folder.length() >= 5 && folder.substr(folder.length() - 5) == "-hard") {
            size_t dashPos = folder.rfind("-");
            if (dashPos != std::string::npos) {
                folder = folder.substr(0, dashPos);
                baseSongName = folder;
            }
        }
        
        SONG = Song::loadFromJson(songName, folder);
        if (!SONG.validScore) {
            std::cerr << "Failed to load song data" << std::endl;
        return;
    }
    
        Conductor::changeBPM(static_cast<float>(SONG.bpm));
        curSong = songName;

        if (vocals != nullptr) {
            delete vocals;
            vocals = nullptr;
        }
        if (inst != nullptr) {
            delete inst;
            inst = nullptr;
        }

        vocalsPath = SONG.needsVoices ? "assets/songs/" + baseSongName + "/Voices" + soundExt : "";
        instPath = "assets/songs/" + baseSongName + "/Inst" + soundExt;
        
    } catch (const std::exception& ex) {
        std::cerr << "Error generating song: " << ex.what() << std::endl;
    }
}

void PlayState::startSong() {
    startingSong = false;
    musicStartTicks = SDL_GetTicks();
    
    if (!vocalsPath.empty()) {
        vocals = new flixel::FlxSound();
        if (!vocals->loadEmbedded(vocalsPath)) {
            std::cerr << "Failed to load vocals: " << vocalsPath << std::endl;
            delete vocals;
            vocals = nullptr;
        } else {
            vocals->play();
        }
    }
    
    inst = new flixel::FlxSound();
    if (!inst->loadEmbedded(instPath)) {
        std::cerr << "Failed to load instrumentals: " << instPath << std::endl;
        delete inst;
        inst = nullptr;
    } else {
        inst->play();
    }
}

void PlayState::startCountdown() {
    startedCountdown = true;

    generateStaticArrows(0);
    generateStaticArrows(1);
}

void PlayState::draw() {
    for (auto arrow : strumLineNotes) {
        if (arrow && arrow->visible) {
            arrow->draw();
        }
    }

    for (auto note : notes) {
        if (note && note->visible) {
            note->draw();
        }
    }

    scoreText->draw();

    if (subState) {
        subState->draw();
    }
}

void PlayState::generateStaticArrows(int player) {
    int windowWidth = flixel::FlxG::width;
    int windowHeight = flixel::FlxG::height;
    
    float yPos = GameConfig::getInstance()->isDownscroll() ? (windowHeight - 150.0f) : 50.0f;
    
    if (!Note::noteFrames) {
        std::cerr << "ERROR: Note assets not loaded!" << std::endl;
        return;
    }
    
    for (int i = 0; i < 4; i++) {
        flixel::FlxSprite* babyArrow = new flixel::FlxSprite();
        babyArrow->setPosition(0, yPos);
        
        babyArrow->texture = Note::noteFrames->texture;
        babyArrow->ownsTexture = false;
        babyArrow->frames = Note::noteFrames;
        babyArrow->animation = new flixel::animation::FlxAnimationController();
        
        if (!Note::noteFrames->frames.empty()) {
            const auto& firstFrame = Note::noteFrames->frames[0];
            babyArrow->sourceRect = firstFrame.rect;
            babyArrow->frameWidth = firstFrame.sourceSize.w;
            babyArrow->frameHeight = firstFrame.sourceSize.h;
            babyArrow->width = static_cast<float>(babyArrow->frameWidth);
            babyArrow->height = static_cast<float>(babyArrow->frameHeight);
        }
        
        std::string staticFrame, pressPrefix, confirmPrefix;
        switch (i) {
            case 0:
                staticFrame = "arrowLEFT";
                pressPrefix = "left press";
                confirmPrefix = "left confirm";
                break;
            case 1:
                staticFrame = "arrowDOWN";
                pressPrefix = "down press";
                confirmPrefix = "down confirm";
                break;
            case 2:
                staticFrame = "arrowUP";
                pressPrefix = "up press";
                confirmPrefix = "up confirm";
                break;
            case 3:
                staticFrame = "arrowRIGHT";
                pressPrefix = "right press";
                confirmPrefix = "right confirm";
                break;
        }
        
        auto staticFrames = Note::noteFrames->getFramesByPrefix(staticFrame);
        auto pressedFrames = Note::noteFrames->getFramesByPrefix(pressPrefix);
        auto confirmFrames = Note::noteFrames->getFramesByPrefix(confirmPrefix);
        
        if (!staticFrames.empty()) {
            babyArrow->animation->addByPrefix("static", staticFrames, 24, false);
        }
        if (!pressedFrames.empty()) {
            babyArrow->animation->addByPrefix("pressed", pressedFrames, 24, false);
        }
        if (!confirmFrames.empty()) {
            babyArrow->animation->addByPrefix("confirm", confirmFrames, 24, false);
        }
        
        babyArrow->setScale(0.7f, 0.7f);
        babyArrow->updateHitbox();
        
        babyArrow->animation->play("static");
        babyArrow->x += Note::swagWidth * i;
        babyArrow->x += 50.0f;
        babyArrow->x += (static_cast<float>(windowWidth) / 2.0f) * player;
        
        babyArrow->visible = true;

        if (camHUD) {
            babyArrow->camera = camHUD;
        }

        strumLineNotes.push_back(babyArrow);
    }
}

void PlayState::generateNotes() {
    unspawnNotes.clear();
    notes.clear();

    if (SONG.notes.empty()) {
        std::cout << "Warning: No song sections found!" << std::endl;
        return;
    }

    std::cout << "Generating notes from " << SONG.notes.size() << " sections" << std::endl;

    int totalNotes = 0;
    int currentSection = 0;
    
    for (const auto& section : SONG.notes) {
        currentSection++;
    }

    currentSection = 0;
    for (const auto& section : SONG.notes) {
        for (const auto& noteData : section.sectionNotes) {
            if (noteData.size() >= 2) {
                float strumTime = noteData[0];
                int noteType = static_cast<int>(noteData[1]);
                
                if (noteType < 0 || noteType > 7) {
                    std::cout << "Warning: Invalid note type " << noteType << " in section " << currentSection << std::endl;
                    continue;
                }
                
                bool mustPress = section.mustHitSection;
                if (noteType >= 4) {
                    mustPress = !section.mustHitSection;
                    noteType = noteType % 4;
                }

                bool sustainNote = noteData.size() > 3 && noteData[3] > 0;
                float sustainLength = sustainNote ? noteData[3] : 0;

                Note* prevNote = nullptr;
                if (sustainNote && !unspawnNotes.empty()) {
                    prevNote = unspawnNotes.back();
                }

                Note* note = new Note(strumTime, noteType, prevNote, sustainNote);
                note->mustPress = mustPress;
                note->sustainLength = sustainLength;
                
                if (mustPress) {
                    note->x += static_cast<float>(flixel::FlxG::width) / 2.0f;
                }
                
                if (camGame) {
                    note->camera = camGame;
                }
                
                unspawnNotes.push_back(note);
                totalNotes++;
            }
        }
        currentSection++;
    }

    std::sort(unspawnNotes.begin(), unspawnNotes.end(),
        [](Note* a, Note* b) {
            return a->strumTime < b->strumTime;
        });
}

void PlayState::destroy() {
    FunkinState::destroy();
}

void PlayState::updateScoreText() {
    std::string text = "Score: " + std::to_string(score) + " Misses: " + std::to_string(misses);
    scoreText->setText(text);
}

void PlayState::goodNoteHit(Note* note) {
    if (!note->wasGoodHit) {
        note->wasGoodHit = true;
        
        if (note->noteData >= 0 && note->noteData < 4) {
            int arrowIndex = note->noteData + 4;
            if (arrowIndex < strumLineNotes.size() && strumLineNotes[arrowIndex]) {
                if (strumLineNotes[arrowIndex]->animation) {
                    strumLineNotes[arrowIndex]->animation->play("confirm");
                }
            }
        }

        combo++;
        score += 350;
        updateScoreText();
        
        note->kill = true;
    }
}

void PlayState::noteMiss(int direction) {
    combo = 0;
    misses++;
    score -= 10;
    if (score < 0) score = 0;
    updateScoreText();
}

void PlayState::loadKeybinds() {
    std::ifstream configFile("assets/data/config.json");
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config.json" << std::endl;
        return;
    }

    nlohmann::json config;
    try {
        configFile >> config;
    } catch (const std::exception& ex) {
        std::cerr << "Failed to parse config.json: " << ex.what() << std::endl;
        return;
    }

    if (config.contains("mainBinds")) {
        auto mainBinds = config["mainBinds"];
        arrowKeys[0].primary = getScancodeFromString(mainBinds["left"].get<std::string>());
        arrowKeys[1].primary = getScancodeFromString(mainBinds["down"].get<std::string>());
        arrowKeys[2].primary = getScancodeFromString(mainBinds["up"].get<std::string>());
        arrowKeys[3].primary = getScancodeFromString(mainBinds["right"].get<std::string>());
    }

    if (config.contains("altBinds")) {
        auto altBinds = config["altBinds"];
        arrowKeys[0].alternate = getScancodeFromString(altBinds["left"].get<std::string>());
        arrowKeys[1].alternate = getScancodeFromString(altBinds["down"].get<std::string>());
        arrowKeys[2].alternate = getScancodeFromString(altBinds["up"].get<std::string>());
        arrowKeys[3].alternate = getScancodeFromString(altBinds["right"].get<std::string>());
    }

    if (config.contains("nxBinds")) {
        auto nxBinds = config["nxBinds"];
        nxArrowKeys[0].primary = getButtonFromString(nxBinds["left"].get<std::string>());
        nxArrowKeys[1].primary = getButtonFromString(nxBinds["down"].get<std::string>());
        nxArrowKeys[2].primary = getButtonFromString(nxBinds["up"].get<std::string>());
        nxArrowKeys[3].primary = getButtonFromString(nxBinds["right"].get<std::string>());
    }

    if (config.contains("nxAltBinds")) {
        auto nxAltBinds = config["nxAltBinds"];
        nxArrowKeys[0].alternate = getButtonFromString(nxAltBinds["left"].get<std::string>());
        nxArrowKeys[1].alternate = getButtonFromString(nxAltBinds["down"].get<std::string>());
        nxArrowKeys[2].alternate = getButtonFromString(nxAltBinds["up"].get<std::string>());
        nxArrowKeys[3].alternate = getButtonFromString(nxAltBinds["right"].get<std::string>());
    }
}

SDL_Scancode PlayState::getScancodeFromString(const std::string& keyName) {
    static const std::map<std::string, SDL_Scancode> keyMap = {
        {"A", SDL_SCANCODE_A},
        {"B", SDL_SCANCODE_B},
        {"C", SDL_SCANCODE_C},
        {"D", SDL_SCANCODE_D},
        {"E", SDL_SCANCODE_E},
        {"F", SDL_SCANCODE_F},
        {"G", SDL_SCANCODE_G},
        {"H", SDL_SCANCODE_H},
        {"I", SDL_SCANCODE_I},
        {"J", SDL_SCANCODE_J},
        {"K", SDL_SCANCODE_K},
        {"L", SDL_SCANCODE_L},
        {"M", SDL_SCANCODE_M},
        {"N", SDL_SCANCODE_N},
        {"O", SDL_SCANCODE_O},
        {"P", SDL_SCANCODE_P},
        {"Q", SDL_SCANCODE_Q},
        {"R", SDL_SCANCODE_R},
        {"S", SDL_SCANCODE_S},
        {"T", SDL_SCANCODE_T},
        {"U", SDL_SCANCODE_U},
        {"V", SDL_SCANCODE_V},
        {"W", SDL_SCANCODE_W},
        {"X", SDL_SCANCODE_X},
        {"Y", SDL_SCANCODE_Y},
        {"Z", SDL_SCANCODE_Z},
        {"ArrowLeft", SDL_SCANCODE_LEFT},
        {"ArrowRight", SDL_SCANCODE_RIGHT},
        {"ArrowUp", SDL_SCANCODE_UP},
        {"ArrowDown", SDL_SCANCODE_DOWN},
        {"Space", SDL_SCANCODE_SPACE},
        {"Enter", SDL_SCANCODE_RETURN},
        {"Escape", SDL_SCANCODE_ESCAPE},
        {"Left", SDL_SCANCODE_LEFT},
        {"Right", SDL_SCANCODE_RIGHT},
        {"Up", SDL_SCANCODE_UP},
        {"Down", SDL_SCANCODE_DOWN}
    };

    auto it = keyMap.find(keyName);
    if (it != keyMap.end()) {
        return it->second;
    }

    std::cout << "Unknown key name: " << keyName << std::endl;
    return SDL_SCANCODE_UNKNOWN;
}

SDL_GameControllerButton PlayState::getButtonFromString(const std::string& buttonName) {
    static const std::map<std::string, SDL_GameControllerButton> buttonMap = {
        {"A", SDL_CONTROLLER_BUTTON_A},
        {"B", SDL_CONTROLLER_BUTTON_B},
        {"X", SDL_CONTROLLER_BUTTON_X},
        {"Y", SDL_CONTROLLER_BUTTON_Y},
        {"DPAD_LEFT", SDL_CONTROLLER_BUTTON_DPAD_LEFT},
        {"DPAD_RIGHT", SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
        {"DPAD_UP", SDL_CONTROLLER_BUTTON_DPAD_UP},
        {"DPAD_DOWN", SDL_CONTROLLER_BUTTON_DPAD_DOWN},
        {"LEFT_SHOULDER", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"RIGHT_SHOULDER", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"LEFT_TRIGGER", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"RIGHT_TRIGGER", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"ZL", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"ZR", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"LT", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {"RT", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
        {"LEFT_STICK", SDL_CONTROLLER_BUTTON_LEFTSTICK},
        {"RIGHT_STICK", SDL_CONTROLLER_BUTTON_RIGHTSTICK},
        {"START", SDL_CONTROLLER_BUTTON_START},
        {"BACK", SDL_CONTROLLER_BUTTON_BACK}
    };

    auto it = buttonMap.find(buttonName);
    if (it != buttonMap.end()) {
        return it->second;
    }

    std::cout << "Unknown button name: " << buttonName << std::endl;
    return SDL_CONTROLLER_BUTTON_INVALID;
}

void PlayState::handleOpponentNoteHit(float deltaTime) {
    static float animationTimer = 0.0f;
    static bool isAnimating = false;
    static int currentArrowIndex = -1;

    for (auto note : notes) {
        if (note && !note->mustPress && !note->wasGoodHit) {
            float timeDiff = note->strumTime - Conductor::songPosition;
            
            if (timeDiff <= 45.0f && timeDiff >= -Conductor::safeZoneOffset) {
                note->canBeHit = true;
                
                int arrowIndex = note->noteData;
                if (arrowIndex < strumLineNotes.size() && strumLineNotes[arrowIndex]) {
                    if (strumLineNotes[arrowIndex]->animation) {
                        strumLineNotes[arrowIndex]->animation->play("confirm");
                    }
                    isAnimating = true;
                    currentArrowIndex = arrowIndex;
                    animationTimer = 0.0f;
                }
                
                note->wasGoodHit = true;
                note->kill = true;
            }
        }
    }

    if (isAnimating) {
        animationTimer += deltaTime;
        if (animationTimer >= 0.1f) {
            if (currentArrowIndex >= 0 && currentArrowIndex < strumLineNotes.size() && strumLineNotes[currentArrowIndex]) {
                if (strumLineNotes[currentArrowIndex]->animation) {
                    strumLineNotes[currentArrowIndex]->animation->play("static");
                }
            }
            isAnimating = false;
            currentArrowIndex = -1;
        }
    }
}

bool PlayState::isKeyJustPressed(int key) {
    if (key < 0 || key >= 4) return false;
    return flixel::FlxG::keys.keys[arrowKeys[key].primary].justPressed() || flixel::FlxG::keys.keys[arrowKeys[key].alternate].justPressed();
}

bool PlayState::isKeyJustReleased(int key) {
    if (key < 0 || key >= 4) return false;
    return flixel::FlxG::keys.keys[arrowKeys[key].primary].justReleased() || flixel::FlxG::keys.keys[arrowKeys[key].alternate].justReleased();
}

bool PlayState::isKeyPressed(int key) {
    if (key < 0 || key >= 4) return false;
    return flixel::FlxG::keys.keys[arrowKeys[key].primary].pressed() || flixel::FlxG::keys.keys[arrowKeys[key].alternate].pressed();
}

bool PlayState::isNXButtonJustPressed(int key) {
    return false;
}

bool PlayState::isNXButtonJustReleased(int key) {
    return false;
}

void PlayState::updateCameraZoom() {}

void PlayState::setupHUDCamera() {
    if (camHUD && scoreText) {
        scoreText->camera = camHUD;
    }
} 