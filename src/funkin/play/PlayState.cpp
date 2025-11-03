#include "PlayState.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>
#include "song/Song.h"
#include "song/Conductor.h"
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
    stage = nullptr;
    boyfriend = nullptr;
    gf = nullptr;
    dad = nullptr;
    characterManager = nullptr;
    opponentStrumline = nullptr;
    playerStrumline = nullptr;
    noteManager = nullptr;
    noteHitHandler = nullptr;
    noteUpdateHandler = nullptr;
    gameplayManager = nullptr;
    cameraManager = nullptr;
    pauseHandler = nullptr;
    renderer = nullptr;
    controls = new Controls();
    healthBar = nullptr;
    NoteSprite::loadAssets();
    scoreText = new flixel::FlxText(0, 0, 0, "");
    scoreText->setFont("assets/fonts/vcr.ttf");
    scoreText->setSize(16);
    scoreText->setBorderStyle(flixel::FlxTextBorderStyle::OUTLINE_FAST, {0, 0, 0, 255}, 1.0f);
    scoreText->scrollFactor.x = 0.0f;
    scoreText->scrollFactor.y = 0.0f;
    
    int windowWidth = flixel::FlxG::width;
    int windowHeight = flixel::FlxG::height;
    scoreText->setPosition(static_cast<float>(windowWidth / 2 - 100), static_cast<float>(windowHeight - 50));
    scoreText->setText("Score: 0 | Misses: 0 | Accuracy: 100.00%");

    if (controls) {
        controls->loadFromConfig("assets/data/config.json");
    }
    
    startingSong = false;
    startedCountdown = false;
    musicStartTicks = 0;
    gfSpeed = 1;
    
    countdown = new Countdown();
    popUpStuff = new PopUpStuff();
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
    
    if (cameraManager != nullptr) {
        delete cameraManager;
        cameraManager = nullptr;
    }
    if (pauseHandler != nullptr) {
        delete pauseHandler;
        pauseHandler = nullptr;
    }
    if (renderer != nullptr) {
        delete renderer;
        renderer = nullptr;
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
    
    if (characterManager != nullptr) {
        delete characterManager;
        characterManager = nullptr;
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
    
    if (opponentStrumline != nullptr) {
        delete opponentStrumline;
        opponentStrumline = nullptr;
    }
    if (playerStrumline != nullptr) {
        delete playerStrumline;
        playerStrumline = nullptr;
    }
    if (controls != nullptr) {
        delete controls;
        controls = nullptr;
    }
    if (noteManager != nullptr) {
        delete noteManager;
        noteManager = nullptr;
    }
    if (noteHitHandler != nullptr) {
        delete noteHitHandler;
        noteHitHandler = nullptr;
    }
    if (noteUpdateHandler != nullptr) {
        delete noteUpdateHandler;
        noteUpdateHandler = nullptr;
    }
    if (gameplayManager != nullptr) {
        delete gameplayManager;
        gameplayManager = nullptr;
    }
    if (healthBar != nullptr) {
        delete healthBar;
        healthBar = nullptr;
    }
    
    delete scoreText;
    
    if (countdown != nullptr) {
        delete countdown;
        countdown = nullptr;
    }
    if (popUpStuff != nullptr) {
        delete popUpStuff;
        popUpStuff = nullptr;
    }
    
    NoteSprite::unloadAssets();
    destroy();
}

void PlayState::loadSongConfig() {
    std::ifstream configFile("assets/data/config.json");
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config.json" << std::endl;
        SONG = SongLoader::loadSong("fnf2");
        curSong = "fnf2";
        SongLoader::loadSongAudio("fnf2", inst, vocals, SONG);
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
            SONG = SongLoader::loadSong(fullSongName);
            curSong = fullSongName;
            SongLoader::loadSongAudio(fullSongName, inst, vocals, SONG);
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
    
    cameraManager = new CameraManager(camGame, 1.05f);
    cameraManager->initialize(boyfriend, dad);
    characterManager = new CharacterManager(gf, dad, boyfriend, gfSpeed);
    
    int windowWidth = flixel::FlxG::width;
    int windowHeight = flixel::FlxG::height;
    
    float healthBarY = GameConfig::getInstance()->isDownscroll() ? (windowHeight * 0.11f) : (windowHeight * 0.89f);
    float healthBarX = (windowWidth - 601) / 2.0f;
    
    healthBar = new HealthBar(healthBarX, healthBarY, camHUD);
    healthBar->setIcons("bf", "dad");
    healthBar->setHealth(1.0f);
    
    noteManager = new NoteManager(camHUD);
    noteManager->generateNotes(SONG);
    
    float strumYPos = GameConfig::getInstance()->isDownscroll() ? (windowWidth - 150.0f) : 50.0f;
    float opponentX = 42.0f + 50.0f;
    float playerX = opponentX + (windowWidth / 2.0f);
    
    opponentStrumline = new Strumline(opponentX, strumYPos, 0, camHUD);
    playerStrumline = new Strumline(playerX, strumYPos, 1, camHUD);
    
    noteHitHandler = new NoteHitHandler(controls, noteManager, playerStrumline, boyfriend, healthBar, popUpStuff, scoreText, vocals, camHUD);
    gameplayManager = new GameplayManager(noteManager, opponentStrumline, dad, gf, healthBar, vocals, SONG);
    noteUpdateHandler = new NoteUpdateHandler(noteManager, noteHitHandler, gameplayManager, gf);
    pauseHandler = new PauseHandler();
    renderer = new PlayStateRenderer();
    
    startCountdown();
}

void PlayState::update(float elapsed) {
    FunkinState::update(elapsed);
    
    if (pauseHandler) {
        pauseHandler->update(elapsed, inst, vocals, Conductor::songPosition, musicStartTicks, subState,
                            [this](flixel::FlxSubState* s) { this->openSubState(s); },
                            [this]() { this->closeSubState(); });
    }

    if (!subState) {
        if (camGame) {
            camGame->update(elapsed);
        }
        if (camHUD) {
            camHUD->update(elapsed);
        }
        
        if (popUpStuff) {
            popUpStuff->update(elapsed, Conductor::crochet);
        }
        
        if (noteHitHandler) {
            noteHitHandler->handleInput();
        }
        if (gameplayManager) {
            gameplayManager->handleOpponentNotes(elapsed);
        }
        
        if (characterManager) {
            characterManager->update(elapsed);
        }
        
        if (opponentStrumline) {
            opponentStrumline->update(elapsed);
        }
        if (playerStrumline) {
            playerStrumline->update(elapsed);
        }
        
        if (healthBar) {
            healthBar->update(elapsed);
        }

        if (!startingSong && musicStartTicks > 0) {
            Conductor::songPosition = static_cast<float>(SDL_GetTicks() - musicStartTicks);
        } else {
            Conductor::songPosition += elapsed * 1000.0f;
        }

        if (noteUpdateHandler) {
            noteUpdateHandler->updateNotes(elapsed, boyfriend, vocals);
        }

        if (startedCountdown && startingSong && countdown) {
            countdown->update(elapsed);
            
            flixel::FlxSprite* countdownSprite = countdown->getCurrentSprite();
            if (countdownSprite && camHUD) {
                                countdownSprite->camera = camHUD;
            }
            
            if (Conductor::songPosition >= 0 && countdown->isFinished()) {
                startSong();
            }
        }

        if (cameraManager) {
        int curSection = curStep / 16;
            cameraManager->update(elapsed, curSection, SONG);
        }
    }
}

void PlayState::startSong() {
    startingSong = false;
    musicStartTicks = SDL_GetTicks();
    
    if (cameraManager) {
        cameraManager->setCamZooming(true);
    }
    
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
    
    if (countdown) {
        countdown->start(Conductor::crochet);
    }

    Conductor::songPosition = -(Conductor::crochet * 5);
}

void PlayState::draw() {
    if (renderer) {
        renderer->draw(stage, gf, dad, boyfriend, opponentStrumline, playerStrumline,
                      noteManager, healthBar, scoreText, popUpStuff, countdown, subState);
    }
}

void PlayState::destroy() {
    FunkinState::destroy();
}

void PlayState::updateCameraZoom() {}

void PlayState::setupHUDCamera() {
    if (camHUD && scoreText) {
        scoreText->camera = camHUD;
    }
}

void PlayState::beatHit() {
    if (characterManager) {
        characterManager->beatHit(curBeat, curStep, SONG);
    }
} 