#include "PlayState.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>
#include "../game/Song.h"
#include "../game/Conductor.h"
#include <fstream>
#include <map>
#include "../../../external/nlohmann/json.hpp"
#include <flixel/FlxG.h>
#include <flixel/math/FlxMath.h>

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
    missSound1 = nullptr;
    missSound2 = nullptr;
    missSound3 = nullptr;
    stage = nullptr;
    boyfriend = nullptr;
    gf = nullptr;
    dad = nullptr;
    Note::loadAssets();
    scoreText = new flixel::FlxText(0, 0, 0, "");
    scoreText->setFont("assets/fonts/vcr.ttf");
    scoreText->setSize(16);
    scoreText->setBorderStyle(flixel::FlxTextBorderStyle::OUTLINE_FAST, {0, 0, 0, 255}, 1.0f);
    scoreText->scrollFactor.x = 0.0f;
    scoreText->scrollFactor.y = 0.0f;
    
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
    gfSpeed = 1;
    
    sicks = 0;
    goods = 0;
    bads = 0;
    shits = 0;
    
    health = 1.0f;
    healthBarBG = nullptr;
    healthBarRed = nullptr;
    healthBarGreen = nullptr;
    iconP1 = nullptr;
    iconP2 = nullptr;
    
    countdownStep = 0;
    countdownTimer = 0.0f;
    countdownInterval = 0.0f;
    countdownSound = nullptr;
    countdownSprite = nullptr;
    countdownSpriteTimer = 0.0f;
    countdownSpriteStartY = 0.0f;
    
    camFollow = nullptr;
    defaultCamZoom = 1.05f;
    camZooming = false;
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
    
    if (camFollow != nullptr) {
        delete camFollow;
        camFollow = nullptr;
    }
    
    if (camGame != nullptr) {
        delete camGame;
        camGame = nullptr;
    }
    if (camHUD != nullptr) {
        delete camHUD;
        camHUD = nullptr;
    }
    
    if (stage != nullptr) {
        delete stage;
        stage = nullptr;
    }
    
    if (boyfriend != nullptr) {
        delete boyfriend;
        boyfriend = nullptr;
    }
    if (gf != nullptr) {
        delete gf;
        gf = nullptr;
    }
    if (dad != nullptr) {
        delete dad;
        dad = nullptr;
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
    
    if (countdownSound != nullptr) {
        delete countdownSound;
        countdownSound = nullptr;
    }
    if (countdownSprite != nullptr) {
        delete countdownSprite;
        countdownSprite = nullptr;
    }
    
    if (healthBarBG != nullptr) {
        delete healthBarBG;
        healthBarBG = nullptr;
    }
    if (healthBarRed != nullptr) {
        delete healthBarRed;
        healthBarRed = nullptr;
    }
    if (healthBarGreen != nullptr) {
        delete healthBarGreen;
        healthBarGreen = nullptr;
    }
    if (iconP1 != nullptr) {
        delete iconP1;
        iconP1 = nullptr;
    }
    if (iconP2 != nullptr) {
        delete iconP2;
        iconP2 = nullptr;
    }
    
    for (auto ratingSprite : ratingSprites) {
        if (ratingSprite != nullptr) {
            delete ratingSprite;
        }
    }
    ratingSprites.clear();
    ratingTimers.clear();
    
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
    
    missSound1 = new flixel::FlxSound();
    missSound1->loadAsChunk("assets/sounds/missnote1" + soundExt, false, false);
    missSound2 = new flixel::FlxSound();
    missSound2->loadAsChunk("assets/sounds/missnote2" + soundExt, false, false);
    missSound3 = new flixel::FlxSound();
    missSound3->loadAsChunk("assets/sounds/missnote3" + soundExt, false, false);
    
    std::string stageName = SONG.stage;
    if (stageName.empty()) {
        stageName = Stage::getStageFromSong(SONG.song);
        SONG.stage = stageName;
    }
    
    stage = new Stage(stageName);
    if (stage && camGame) {
        for (auto sprite : stage->getSprites()) {
            if (sprite) {
                sprite->camera = camGame;
            }
        }
    }
    
    gf = new Character(400, 130, "gf", false);
    gf->scrollFactor.set(0.95f, 0.95f);
    if (camGame) {
        gf->camera = camGame;
    }
    
    dad = new Character(100, 100, "dad", false);
    if (camGame) {
        dad->camera = camGame;
    }
    
    boyfriend = new Character(770, 450, "bf", true);
    if (camGame) {
        boyfriend->camera = camGame;
    }
    
    camFollow = new flixel::FlxObject(boyfriend->getMidpoint().x - 100, boyfriend->getMidpoint().y - 100, 1, 1);
    
    if (camGame) {
        camGame->follow(camFollow, flixel::FlxCameraFollowStyle::LOCKON, 0.04f);
        camGame->zoom = defaultCamZoom;
        camGame->focusOn(camFollow->getMidpoint());
    }
    
    int windowWidth = flixel::FlxG::width;
    int windowHeight = flixel::FlxG::height;
    
    healthBarBG = new flixel::FlxSprite();
    healthBarBG->makeGraphic(601, 19, {0, 0, 0, 255});
    healthBarBG->y = GameConfig::getInstance()->isDownscroll() ? (windowHeight * 0.11f) : (windowHeight * 0.89f);
    healthBarBG->screenCenter(flixel::util::FlxAxes::X);
    healthBarBG->scrollFactor.x = 0.0f;
    healthBarBG->scrollFactor.y = 0.0f;
    if (camHUD) {
        healthBarBG->camera = camHUD;
    }
    
    healthBarRed = new flixel::FlxSprite();
    healthBarRed->makeGraphic(593, 11, {255, 0, 0, 255});
    healthBarRed->setPosition(healthBarBG->x + 4, healthBarBG->y + 4);
    healthBarRed->scrollFactor.x = 0.0f;
    healthBarRed->scrollFactor.y = 0.0f;
    if (camHUD) {
        healthBarRed->camera = camHUD;
    }
    
    healthBarGreen = new flixel::FlxSprite();
    healthBarGreen->makeGraphic(593, 11, {0, 255, 0, 255});
    healthBarGreen->setPosition(healthBarBG->x + 4, healthBarBG->y + 4);
    healthBarGreen->scrollFactor.x = 0.0f;
    healthBarGreen->scrollFactor.y = 0.0f;
    if (camHUD) {
        healthBarGreen->camera = camHUD;
    }
    
    iconP1 = new HealthIcon("bf", true);
    iconP1->y = healthBarBG->y - 75;
    iconP1->scrollFactor.x = 0.0f;
    iconP1->scrollFactor.y = 0.0f;
    if (camHUD) {
        iconP1->camera = camHUD;
    }
    
    iconP2 = new HealthIcon("dad", false);
    iconP2->y = healthBarBG->y - 75;
    iconP2->scrollFactor.x = 0.0f;
    iconP2->scrollFactor.y = 0.0f;
    if (camHUD) {
        iconP2->camera = camHUD;
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
        if (camGame) {
            camGame->update(elapsed);
        }
        if (camHUD) {
            camHUD->update(elapsed);
        }
        
        for (size_t i = 0; i < ratingSprites.size(); ) {
            ratingTimers[i] += elapsed;
            
            float startDelay;
            if (i == 0) {
                startDelay = Conductor::crochet * 0.001f;
            } else if (i == 1) {
                startDelay = Conductor::crochet * 0.001f;
            } else {
                startDelay = Conductor::crochet * 0.002f;
            }
            
            float fadeTime = 0.2f;
            float totalTime = startDelay + fadeTime;
            
            if (ratingTimers[i] >= totalTime) {
                delete ratingSprites[i];
                ratingSprites.erase(ratingSprites.begin() + i);
                ratingTimers.erase(ratingTimers.begin() + i);
            } else {
                ratingSprites[i]->update(elapsed);
                
                if (ratingTimers[i] < startDelay) {
                    ratingSprites[i]->alpha = 1.0f;
                } else {
                    float fadeProgress = (ratingTimers[i] - startDelay) / fadeTime;
                    ratingSprites[i]->alpha = 1.0f - fadeProgress;
                }
                i++;
            }
        }
        
        handleInput();
        handleOpponentNoteHit(elapsed);
        
        if (gf) {
            gf->update(elapsed);
        }
        if (dad) {
            dad->update(elapsed);
        }
        if (boyfriend) {
            boyfriend->update(elapsed);
        }
        
        if (health > 2.0f) health = 2.0f;
        if (health < 0.0f) health = 0.0f;
        
        if (healthBarGreen && healthBarRed && iconP1 && iconP2) {
            float healthPercent = (health / 2.0f) * 100.0f;
            float barWidth = 593.0f;
            
            float greenWidth = (healthPercent / 100.0f) * barWidth;
            healthBarGreen->setScale((healthPercent / 100.0f), 1.0f);
            healthBarGreen->x = healthBarBG->x + 4 + barWidth - greenWidth;
            
            int iconOffset = 26;
            float iconX = healthBarBG->x + 4 + barWidth - greenWidth;
            iconP1->x = iconX + (150 * iconP1->scale.x - 150) / 2 - iconOffset;
            iconP2->x = iconX - (150 * iconP2->scale.x) / 2 - iconOffset * 2;
            
            iconP1->update(elapsed);
            iconP2->update(elapsed);
            
            int iconWidth = iconP1->fullIconWidth / 2;
            if (healthPercent <= 20.0f) {
                iconP1->sourceRect.x = iconWidth;
            } else {
                iconP1->sourceRect.x = 0;
            }
            
            iconWidth = iconP2->fullIconWidth / 2;
            if (healthPercent >= 80.0f) {
                iconP2->sourceRect.x = iconWidth;
            } else {
                iconP2->sourceRect.x = 0;
            }
        }

        for (auto arrow : strumLineNotes) {
            if (arrow) {
                arrow->update(elapsed);
                if (arrow->animation) {
                    arrow->animation->update(elapsed);
                }
            }
        }
        
        for (int i = 0; i < 8; i++) {
            if (i < strumLineNotes.size() && strumLineNotes[i] && strumLineNotes[i]->animation) {
                auto arrow = strumLineNotes[i];
                if (arrow->animation->current == "confirm" && arrow->animation->finished) {
                    arrow->animation->play("static");
                }
            }
        }
        updateArrowAnimations();

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

                if (note->y < -note->height) {
                    if (note->isSustainNote && note->wasGoodHit) {
                        note->kill = true;
                    }
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

        if (startedCountdown && startingSong) {
            countdownTimer += elapsed;
            
            if (countdownTimer >= countdownInterval) {
                countdownTimer -= countdownInterval;
                
                if (countdownSprite) {
                    delete countdownSprite;
                    countdownSprite = nullptr;
                }
                
                if (dad) dad->dance();
                if (gf) gf->dance();
                if (boyfriend) boyfriend->playAnim("idle", true);
                
                switch (countdownStep) {
                    case 0:
                        if (countdownSound) delete countdownSound;
                        countdownSound = new flixel::FlxSound();
                        countdownSound->loadAsChunk("assets/sounds/intro3" + soundExt, false, false);
                        countdownSound->play();
                        break;
                        
                    case 1: {
                        countdownSprite = new flixel::FlxSprite();
                        countdownSprite->loadGraphic("assets/images/ready.png");
                        if (countdownSprite->texture) {
                            countdownSprite->scrollFactor.x = 0.0f;
                            countdownSprite->scrollFactor.y = 0.0f;
                            countdownSprite->screenCenter();
                            countdownSpriteStartY = countdownSprite->y;
                            countdownSpriteTimer = 0.0f;
                            if (camHUD) {
                                countdownSprite->camera = camHUD;
                            }
                        } else {
                            delete countdownSprite;
                            countdownSprite = nullptr;
                        }
                        
                        if (countdownSound) delete countdownSound;
                        countdownSound = new flixel::FlxSound();
                        countdownSound->loadAsChunk("assets/sounds/intro2" + soundExt, false, false);
                        countdownSound->play();
                        break;
                    }
                        
                    case 2: {
                        countdownSprite = new flixel::FlxSprite();
                        countdownSprite->loadGraphic("assets/images/set.png");
                        if (countdownSprite->texture) {
                            countdownSprite->scrollFactor.x = 0.0f;
                            countdownSprite->scrollFactor.y = 0.0f;
                            countdownSprite->screenCenter();
                            countdownSpriteStartY = countdownSprite->y;
                            countdownSpriteTimer = 0.0f;
                            if (camHUD) {
                                countdownSprite->camera = camHUD;
                            }
                        } else {
                            delete countdownSprite;
                            countdownSprite = nullptr;
                        }
                        
                        if (countdownSound) delete countdownSound;
                        countdownSound = new flixel::FlxSound();
                        countdownSound->loadAsChunk("assets/sounds/intro1" + soundExt, false, false);
                        countdownSound->play();
                        break;
                    }
                        
                    case 3: {
                        countdownSprite = new flixel::FlxSprite();
                        countdownSprite->loadGraphic("assets/images/go.png");
                        if (countdownSprite->texture) {
                            countdownSprite->scrollFactor.x = 0.0f;
                            countdownSprite->scrollFactor.y = 0.0f;
                            countdownSprite->screenCenter();
                            countdownSpriteStartY = countdownSprite->y;
                            countdownSpriteTimer = 0.0f;
                            if (camHUD) {
                                countdownSprite->camera = camHUD;
                            }
                        } else {
                            delete countdownSprite;
                            countdownSprite = nullptr;
                        }
                        
                        if (countdownSound) delete countdownSound;
                        countdownSound = new flixel::FlxSound();
                        countdownSound->loadAsChunk("assets/sounds/introGo" + soundExt, false, false);
                        countdownSound->play();
                        break;
                    }
                        
                    case 4:
                        break;
                }
                
                countdownStep++;
            }
            
            if (countdownSprite) {
                countdownSpriteTimer += elapsed;
                float progress = countdownSpriteTimer / countdownInterval;
                if (progress > 1.0f) progress = 1.0f;
                
                countdownSprite->alpha = 1.0f - progress;
                countdownSprite->y = countdownSpriteStartY + (100.0f * progress);
                
                if (progress >= 1.0f) {
                    delete countdownSprite;
                    countdownSprite = nullptr;
                }
            }
            
            if (Conductor::songPosition >= 0 && countdownStep > 4) {
                startSong();
            }
        }

        int curSection = curStep / 16;
        static int lastSection = -1;
        if (curSection >= 0 && curSection < SONG.notes.size()) {
            if (curSection != lastSection) {
                lastSection = curSection;
            }
            
            bool mustHit = SONG.notes[curSection].mustHitSection;
            
            if (mustHit) {
                if (camFollow && boyfriend) {
                    camFollow->setPosition(boyfriend->getMidpoint().x - 100, boyfriend->getMidpoint().y - 100);
                }
            } else {
                if (camFollow && dad) {
                    camFollow->setPosition(dad->getMidpoint().x + 150, dad->getMidpoint().y - 100);
                }
            }
        }

        if (pauseCooldown > 0) {
            pauseCooldown -= elapsed;
        }

        bool pausePressed = flixel::FlxG::keys.keys[SDL_SCANCODE_RETURN].justPressed() ||
                           flixel::FlxG::gamepads.justPressed(SDL_CONTROLLER_BUTTON_START);
        
        if (pausePressed) {
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
    float closestDistances[4] = {INFINITY, INFINITY, INFINITY, INFINITY};
    std::vector<Note*> toHit;
    
    bool justHitArray[4] = {
        isKeyJustPressed(0),
        isKeyJustPressed(1),
        isKeyJustPressed(2),
        isKeyJustPressed(3)
    };
    
    bool heldArray[4] = {
        isKeyPressed(0),
        isKeyPressed(1),
        isKeyPressed(2),
        isKeyPressed(3)
    };
    
    if (boyfriend && (justHitArray[0] || justHitArray[1] || justHitArray[2] || justHitArray[3])) {
        boyfriend->holdTimer = 0.0f;
    }
    
    for (int i = 0; i < 4; i++) {
        int arrowIndex = i + 4;
        if (justHitArray[i] && arrowIndex < strumLineNotes.size() && strumLineNotes[arrowIndex]) {
            auto arrow = strumLineNotes[arrowIndex];
            if (arrow->animation && arrow->animation->current != "confirm") {
                arrow->animation->play("pressed");
            }
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
        
        toHit.erase(std::remove_if(toHit.begin(), toHit.end(), [lane, distance](Note* n) {
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
                noteMiss(i);
            }
        }
    }
    
    for (int i = 0; i < 4; i++) {
        int arrowIndex = i + 4;
        if (isKeyJustReleased(i) && arrowIndex < strumLineNotes.size() && strumLineNotes[arrowIndex]) {
            auto arrow = strumLineNotes[arrowIndex];
            if (arrow->animation && arrow->animation->current != "confirm") {
                arrow->animation->play("static");
            }
        }
    }
}

void PlayState::updateArrowAnimations() {
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

        vocalsPath = SONG.needsVoices ? "assets/songs/" + baseSongName + "/Voices" + soundExt : "";
        instPath = "assets/songs/" + baseSongName + "/Inst" + soundExt;
        
        if (inst != nullptr) {
            delete inst;
            inst = nullptr;
        }
        inst = new flixel::FlxSound();
        if (!inst->loadEmbedded(instPath, false, false)) {
            std::cerr << "Failed to preload instrumental: " << instPath << std::endl;
            delete inst;
            inst = nullptr;
        }
        if (inst) {
            inst->stop();
        }
        
        if (!vocalsPath.empty()) {
            if (vocals != nullptr) {
                delete vocals;
                vocals = nullptr;
            }
            vocals = new flixel::FlxSound();
            if (!vocals->loadAsChunk(vocalsPath, false, false)) {
                std::cerr << "Failed to preload vocals: " << vocalsPath << std::endl;
                delete vocals;
                vocals = nullptr;
            } else {
                vocals->setChannel(0);
                vocals->setVolume(1.0f);
                vocals->stop();
            }
        }
        
    } catch (const std::exception& ex) {
        std::cerr << "Error generating song: " << ex.what() << std::endl;
    }
}

void PlayState::startSong() {
    startingSong = false;
    musicStartTicks = SDL_GetTicks();
    camZooming = true;
    
    if (inst) {
        inst->play();
    } else {
        std::cerr << "Error: Instrumental not loaded!" << std::endl;
    }
    
    if (vocals) {
        vocals->play();
    }
}

void PlayState::startCountdown() {
    startedCountdown = true;
    countdownStep = 0;
    countdownTimer = 0.0f;
    countdownInterval = Conductor::crochet / 1000.0f;
    countdownSpriteTimer = 0.0f;
    
    Conductor::songPosition = -(Conductor::crochet * 5);

    generateStaticArrows(0);
    generateStaticArrows(1);
}

void PlayState::draw() {
    if (stage) {
        for (auto sprite : stage->getSprites()) {
            if (sprite && sprite->visible) {
                sprite->draw();
            }
        }
    }
    
    if (gf && gf->visible) {
        gf->draw();
    }
    if (dad && dad->visible) {
        dad->draw();
    }
    if (boyfriend && boyfriend->visible) {
        boyfriend->draw();
    }
    
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

    if (healthBarBG && healthBarBG->visible) {
        healthBarBG->draw();
    }
    if (healthBarRed && healthBarRed->visible) {
        healthBarRed->draw();
    }
    if (healthBarGreen && healthBarGreen->visible) {
        healthBarGreen->draw();
    }
    if (iconP1 && iconP1->visible) {
        iconP1->draw();
    }
    if (iconP2 && iconP2->visible) {
        iconP2->draw();
    }

    scoreText->draw();
    
    for (auto ratingSprite : ratingSprites) {
        if (ratingSprite && ratingSprite->visible) {
            ratingSprite->draw();
        }
    }
    
    if (countdownSprite && countdownSprite->visible) {
        countdownSprite->draw();
    }

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
        babyArrow->setPosition(42.0f, yPos);
        
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
        babyArrow->scrollFactor.x = 0.0f;
        babyArrow->scrollFactor.y = 0.0f;

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

                float sustainLength = (noteData.size() > 2) ? noteData[2] : 0;
                
                Note* oldNote = !unspawnNotes.empty() ? unspawnNotes.back() : nullptr;
                
                Note* swagNote = new Note(strumTime, noteType, oldNote, false);
                swagNote->sustainLength = sustainLength;
                swagNote->mustPress = mustPress;
                swagNote->scrollFactor.x = 0.0f;
                swagNote->scrollFactor.y = 0.0f;
                
                if (mustPress) {
                    swagNote->x += static_cast<float>(flixel::FlxG::width) / 2.0f;
                }
                
                if (camHUD) {
                    swagNote->camera = camHUD;
                }
                
                if (sustainLength > 0) {
                    float susLength = sustainLength / Conductor::stepCrochet;
                    int numSustainPieces = static_cast<int>(std::round(susLength));
                    
                    for (int susNote = 0; susNote < numSustainPieces; susNote++) {
                        oldNote = !unspawnNotes.empty() ? unspawnNotes.back() : nullptr;
                        
                        float sustainNoteTime = strumTime + (Conductor::stepCrochet * susNote);
                        Note* sustainNote = new Note(sustainNoteTime, noteType, oldNote, true);
                        sustainNote->mustPress = mustPress;
                        sustainNote->scrollFactor.x = 0.0f;
                        sustainNote->scrollFactor.y = 0.0f;
                        sustainNote->parentNote = swagNote;
                        
                        if (susNote == numSustainPieces - 1 && sustainNote->animation) {
                            sustainNote->animation->play("hold_end");
                            sustainNote->setScale(0.7f, 0.7f);
                            sustainNote->updateHitbox();
                            sustainNote->x += 34.0f;
                        } else {
                            float scaleMultiplier = Conductor::stepCrochet / 100.0f * 1.5f * SONG.speed;
                            sustainNote->setScale(0.7f, 0.7f * scaleMultiplier);
                            sustainNote->originX = 0.0f;
                            sustainNote->originY = 0.0f;
                            sustainNote->x += 34.0f;
                        }
                        
                        sustainNote->yOffset = 30.0f;
                        
                        if (mustPress) {
                            sustainNote->x += static_cast<float>(flixel::FlxG::width) / 2.0f;
                        }
                        
                        if (camHUD) {
                            sustainNote->camera = camHUD;
                        }
                        
                        unspawnNotes.push_back(sustainNote);
                        totalNotes++;
                    }
                }
                
                unspawnNotes.push_back(swagNote);
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
    if (missSound1 != nullptr) {
        delete missSound1;
        missSound1 = nullptr;
    }
    if (missSound2 != nullptr) {
        delete missSound2;
        missSound2 = nullptr;
    }
    if (missSound3 != nullptr) {
        delete missSound3;
        missSound3 = nullptr;
    }
    
    FunkinState::destroy();
}

void PlayState::updateScoreText() {
    std::string text = "Score: " + std::to_string(score) + 
                      " | Misses: " + std::to_string(misses) + 
                      " | Accuracy: ";
    
    int totalNotes = sicks + goods + bads + shits + misses;
    if (totalNotes > 0) {
        float accuracy = ((sicks * 1.0f + goods * 0.75f + bads * 0.5f + shits * 0.25f) / totalNotes) * 100.0f;
        char accStr[16];
        snprintf(accStr, sizeof(accStr), "%.2f%%", accuracy);
        text += accStr;
    } else {
        text += "0.00%";
    }
    
    scoreText->setText(text);
}

void PlayState::popUpScore(const std::string& rating, int comboNum) {
    int windowWidth = flixel::FlxG::width;
    int windowHeight = flixel::FlxG::height;
    
    flixel::FlxSprite* ratingSprite = new flixel::FlxSprite();
    ratingSprite->loadGraphic("assets/images/" + rating + ".png");
    ratingSprite->screenCenter(flixel::util::FlxAxes::XY);
    ratingSprite->x = windowWidth * 0.55f - 40;
    ratingSprite->y -= 60;
    ratingSprite->acceleration.y = 550;
    ratingSprite->velocity.y = -(std::rand() % 36 + 140);
    ratingSprite->velocity.x = -(std::rand() % 11);
    ratingSprite->setScale(0.7f, 0.7f);
    ratingSprite->scrollFactor.x = 0.0f;
    ratingSprite->scrollFactor.y = 0.0f;
    ratingSprite->alpha = 1.0f;
    if (camHUD) {
        ratingSprite->camera = camHUD;
    }
    ratingSprites.push_back(ratingSprite);
    ratingTimers.push_back(0.0f);
    
    if (comboNum >= 10 || comboNum == 0) {
        flixel::FlxSprite* comboSprite = new flixel::FlxSprite();
        comboSprite->loadGraphic("assets/images/combo.png");
        comboSprite->screenCenter(flixel::util::FlxAxes::XY);
        comboSprite->x = windowWidth * 0.55f;
        comboSprite->acceleration.y = 600;
        comboSprite->velocity.y = -150;
        comboSprite->velocity.x = std::rand() % 10 + 1;
        comboSprite->setScale(0.7f, 0.7f);
        comboSprite->scrollFactor.x = 0.0f;
        comboSprite->scrollFactor.y = 0.0f;
        comboSprite->alpha = 1.0f;
        if (camHUD) {
            comboSprite->camera = camHUD;
        }
        ratingSprites.push_back(comboSprite);
        ratingTimers.push_back(0.0f);
        
        int hundreds = comboNum / 100;
        int tens = (comboNum % 100) / 10;
        int ones = comboNum % 10;
        int comboDigits[3] = {hundreds, tens, ones};
        
        for (int i = 0; i < 3; i++) {
            if (comboNum >= 10 || comboNum == 0) {
                flixel::FlxSprite* numSprite = new flixel::FlxSprite();
                numSprite->loadGraphic("assets/images/num" + std::to_string(comboDigits[i]) + ".png");
                numSprite->screenCenter(flixel::util::FlxAxes::XY);
                numSprite->x = windowWidth * 0.55f + (43 * i) - 90;
                numSprite->y += 80;
                numSprite->acceleration.y = std::rand() % 101 + 200;
                numSprite->velocity.y = -(std::rand() % 21 + 140);
                numSprite->velocity.x = ((std::rand() % 11) - 5.0f);
                numSprite->setScale(0.5f, 0.5f);
                numSprite->scrollFactor.x = 0.0f;
                numSprite->scrollFactor.y = 0.0f;
                numSprite->alpha = 1.0f;
                if (camHUD) {
                    numSprite->camera = camHUD;
                }
                ratingSprites.push_back(numSprite);
                ratingTimers.push_back(0.0f);
            }
        }
    }
}

void PlayState::goodNoteHit(Note* note) {
    if (!note->wasGoodHit) {
        note->wasGoodHit = true;
        
        if (vocals) {
            vocals->setVolume(1.0f);
        }
        
        if (note->noteData >= 0 && note->noteData < 4) {
            int arrowIndex = note->noteData + 4;
            if (arrowIndex < strumLineNotes.size() && strumLineNotes[arrowIndex]) {
                if (strumLineNotes[arrowIndex]->animation) {
                    auto arrow = strumLineNotes[arrowIndex];
                    if (arrow->animation->current != "confirm") {
                        arrow->animation->play("confirm");
                    }
                }
            }
            
            if (boyfriend) {
                std::string animToPlay = "";
                switch (note->noteData) {
                    case 0:
                        animToPlay = "singLEFT";
                        break;
                    case 1:
                        animToPlay = "singDOWN";
                        break;
                    case 2:
                        animToPlay = "singUP";
                        break;
                    case 3:
                        animToPlay = "singRIGHT";
                        break;
                }
                if (!animToPlay.empty()) {
                    boyfriend->playAnim(animToPlay, true);
                    boyfriend->holdTimer = 0.0f;
                }
            }
        }

        if (!note->isSustainNote) {
            float noteDiff = std::abs(note->strumTime - Conductor::songPosition);
            float safeZoneOffset = (10.0f / 60.0f) * 1000.0f;
            
            std::string daRating = "sick";
            int ratingScore = 350;
            
            if (noteDiff > safeZoneOffset * 0.9f) {
                daRating = "shit";
                ratingScore = 50;
                shits++;
            }
            else if (noteDiff > safeZoneOffset * 0.75f) {
                daRating = "bad";
                ratingScore = 100;
                bads++;
            }
            else if (noteDiff > safeZoneOffset * 0.2f) {
                daRating = "good";
                ratingScore = 200;
                goods++;
            }
            else {
                daRating = "sick";
                ratingScore = 350;
                sicks++;
            }
            
            health += 0.05f;
            combo++;
            score += ratingScore;
            popUpScore(daRating, combo);
            updateScoreText();
            note->kill = true;
        }
    }
}

void PlayState::noteMiss(int direction) {
    if (combo > 5 && gf) {
        gf->playAnim("sad", true);
    }
    
    health -= 0.1f;
    
    combo = 0;
    misses++;
    score -= 10;
    if (score < 0) score = 0;
    updateScoreText();
    
    if (vocals) {
        vocals->setVolume(0.0f);
    }
    
    if (boyfriend && direction >= 0 && direction < 4) {
        std::string animToPlay = "";
        switch (direction) {
            case 0:
                animToPlay = "singLEFTmiss";
                break;
            case 1:
                animToPlay = "singDOWNmiss";
                break;
            case 2:
                animToPlay = "singUPmiss";
                break;
            case 3:
                animToPlay = "singRIGHTmiss";
                break;
        }
        if (!animToPlay.empty()) {
            boyfriend->playAnim(animToPlay, true);
            boyfriend->holdTimer = 0.0f;
        }
    }
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> missNoteDist(1, 3);
    static std::uniform_real_distribution<> volumeDist(0.5, 0.8);
    
    int missNoteNum = missNoteDist(gen);
    float volume = static_cast<float>(volumeDist(gen));
    
    flixel::FlxSound* selectedSound = nullptr;
    switch(missNoteNum) {
        case 1: selectedSound = missSound1; break;
        case 2: selectedSound = missSound2; break;
        case 3: selectedSound = missSound3; break;
    }
    
    if (selectedSound) {
        selectedSound->setVolume(volume);
        selectedSound->play(true);
    }
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
    for (auto note : notes) {
        if (note && !note->mustPress && !note->wasGoodHit) {
            float timeDiff = note->strumTime - Conductor::songPosition;
            
            if (timeDiff <= 45.0f && timeDiff >= -Conductor::safeZoneOffset) {
                note->canBeHit = true;
                
                if (vocals && SONG.needsVoices) {
                    vocals->setVolume(1.0f);
                }
                
                int arrowIndex = note->noteData;
                if (arrowIndex >= 0 && arrowIndex < 4 && arrowIndex < strumLineNotes.size()) {
                    if (strumLineNotes[arrowIndex] && strumLineNotes[arrowIndex]->animation) {
                        auto arrow = strumLineNotes[arrowIndex];
                        if (arrow->animation->current != "confirm") {
                            arrow->animation->play("confirm");
                        }
                    }
                }
                
                if (dad) {
                    std::string animToPlay = "";
                    switch (note->noteData) {
                        case 0:
                            animToPlay = "singLEFT";
                            break;
                        case 1:
                            animToPlay = "singDOWN";
                            break;
                        case 2:
                            animToPlay = "singUP";
                            break;
                        case 3:
                            animToPlay = "singRIGHT";
                            break;
                    }
                    if (!animToPlay.empty()) {
                        dad->playAnim(animToPlay, true);
                        dad->holdTimer = 0.0f;
                    }
                }
                
                note->wasGoodHit = true;
                note->kill = true;
            }
        }
    }
}

bool PlayState::isKeyJustPressed(int key) {
    if (key < 0 || key >= 4) return false;
    
    bool keyboardPressed = flixel::FlxG::keys.keys[arrowKeys[key].primary].justPressed() || 
                          flixel::FlxG::keys.keys[arrowKeys[key].alternate].justPressed();
    
    bool controllerPressed = flixel::FlxG::gamepads.justPressed(nxArrowKeys[key].primary) ||
                            flixel::FlxG::gamepads.justPressed(nxArrowKeys[key].alternate);
    
    return keyboardPressed || controllerPressed;
}

bool PlayState::isKeyJustReleased(int key) {
    if (key < 0 || key >= 4) return false;
    
    bool keyboardReleased = flixel::FlxG::keys.keys[arrowKeys[key].primary].justReleased() || 
                           flixel::FlxG::keys.keys[arrowKeys[key].alternate].justReleased();
    
    bool controllerReleased = flixel::FlxG::gamepads.justReleased(nxArrowKeys[key].primary) ||
                             flixel::FlxG::gamepads.justReleased(nxArrowKeys[key].alternate);
    
    return keyboardReleased || controllerReleased;
}

bool PlayState::isKeyPressed(int key) {
    if (key < 0 || key >= 4) return false;
    
    bool keyboardPressed = flixel::FlxG::keys.keys[arrowKeys[key].primary].pressed() || 
                          flixel::FlxG::keys.keys[arrowKeys[key].alternate].pressed();
    
    bool controllerPressed = flixel::FlxG::gamepads.pressed(nxArrowKeys[key].primary) ||
                            flixel::FlxG::gamepads.pressed(nxArrowKeys[key].alternate);
    
    return keyboardPressed || controllerPressed;
}

bool PlayState::isNXButtonJustPressed(int key) {
    if (key < 0 || key >= 4) return false;
    return flixel::FlxG::gamepads.justPressed(nxArrowKeys[key].primary) ||
           flixel::FlxG::gamepads.justPressed(nxArrowKeys[key].alternate);
}

bool PlayState::isNXButtonJustReleased(int key) {
    if (key < 0 || key >= 4) return false;
    return flixel::FlxG::gamepads.justReleased(nxArrowKeys[key].primary) ||
           flixel::FlxG::gamepads.justReleased(nxArrowKeys[key].alternate);
}

void PlayState::updateCameraZoom() {}

void PlayState::setupHUDCamera() {
    if (camHUD && scoreText) {
        scoreText->camera = camHUD;
    }
}

void PlayState::beatHit() {
    if (gf && curBeat % gfSpeed == 0) {
        gf->dance();
    }
    
    if (boyfriend && boyfriend->animation) {
        std::string currentAnim = boyfriend->animation->current;
        bool isSinging = (currentAnim.length() >= 4 && currentAnim.substr(0, 4) == "sing");
        
        if (!isSinging) {
            boyfriend->dance();
        }
    }
    
    if (dad && curBeat % 1 == 0) {
        int curSection = curStep / 16;
        if (curSection >= 0 && curSection < SONG.notes.size()) {
            if (SONG.notes[curSection].mustHitSection) {
                dad->dance();
            }
        }
    }
    
} 