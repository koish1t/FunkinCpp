#pragma once

#include <string>
#include <vector>
#include <map>
#include <SDL2/SDL.h>
#include "../FunkinState.h"
#include "../game/Note.h"
#include "../game/Song.h"
#include "../game/Conductor.h"
#include "../game/GameConfig.h"
#include "../substates/PauseSubState.h"
#include "../game/Stage.h"
#include "../game/Character.h"
#include <flixel/FlxSprite.h>
#include <flixel/text/FlxText.h>
#include <flixel/sound/FlxSound.h>
#include <flixel/FlxCamera.h>

class PlayState : public FunkinState {
public:
    static PlayState* instance;
    static SwagSong SONG;
    static flixel::FlxSound* inst;

    PlayState();
    ~PlayState() override;

    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;

    void loadSongConfig();
    void generateSong(std::string dataPath);
    void startSong();
    void startCountdown();
    void generateStaticArrows(int player);
    void generateNotes();
    void handleInput();
    void updateArrowAnimations();
    void goodNoteHit(Note* note);
    void noteMiss(int direction);
    void handleOpponentNoteHit(float deltaTime);
    void updateScoreText();
    void loadKeybinds();
    void updateCameraZoom();
    void setupHUDCamera();
    void beatHit();

    bool isKeyJustPressed(int key);
    bool isKeyJustReleased(int key);
    bool isKeyPressed(int key);
    bool isNXButtonJustPressed(int key);
    bool isNXButtonJustReleased(int key);
    SDL_Scancode getScancodeFromString(const std::string& keyName);
    SDL_GameControllerButton getButtonFromString(const std::string& buttonName);

private:
    flixel::FlxCamera* camGame;
    flixel::FlxCamera* camHUD;
    flixel::FlxSound* vocals;
    flixel::FlxSound* missSound1;
    flixel::FlxSound* missSound2;
    flixel::FlxSound* missSound3;
    Stage* stage;
    Character* boyfriend;
    Character* gf;
    Character* dad;
    std::vector<flixel::FlxSprite*> strumLineNotes;
    std::vector<Note*> notes;
    std::vector<Note*> unspawnNotes;
    flixel::FlxText* scoreText;
    
    std::string curSong;
    std::string vocalsPath;
    std::string instPath;
    bool startingSong;
    bool startedCountdown;
    float pauseCooldown;
    Uint32 musicStartTicks;
    
    int countdownStep;
    float countdownTimer;
    float countdownInterval;
    flixel::FlxSound* countdownSound;
    flixel::FlxSprite* countdownSprite;
    float countdownSpriteTimer;
    float countdownSpriteStartY;
    
    int score;
    int misses;
    int combo;
    int gfSpeed;
    
    flixel::FlxObject* camFollow;
    float defaultCamZoom;
    bool camZooming;
    
    struct KeyBind {
        SDL_Scancode primary;
        SDL_Scancode alternate;
    };
    
    struct NXKeyBind {
        SDL_GameControllerButton primary;
        SDL_GameControllerButton alternate;
    };
    
    KeyBind arrowKeys[4];
    NXKeyBind nxArrowKeys[4];
    
    static const char* NOTE_STYLES[];
    static const char* NOTE_DIRS[];
};