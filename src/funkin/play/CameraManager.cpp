#include "CameraManager.h"

CameraManager::CameraManager(flixel::FlxCamera* camGame, float defaultZoom)
    : camGame(camGame)
    , camFollow(nullptr)
    , boyfriend(nullptr)
    , dad(nullptr)
    , defaultCamZoom(defaultZoom)
    , camZooming(false)
{
}

CameraManager::~CameraManager() {
    if (camFollow) {
        delete camFollow;
        camFollow = nullptr;
    }
}

void CameraManager::initialize(Character* bf, Character* dadChar) {
    boyfriend = bf;
    dad = dadChar;
    
    if (boyfriend && camGame) {
        camFollow = new flixel::FlxObject(boyfriend->getMidpoint().x - 100, boyfriend->getMidpoint().y - 100, 1, 1);
        camGame->follow(camFollow, flixel::FlxCameraFollowStyle::LOCKON, 0.04f);
        camGame->focusOn(camFollow->getMidpoint());
    }
}

void CameraManager::update(float elapsed, int curSection, const SwagSong& song) {
    if (!camFollow || curSection < 0 || curSection >= song.notes.size()) {
        return;
    }
    
    bool mustHit = song.notes[curSection].mustHitSection;
    
    if (mustHit && boyfriend) {
        camFollow->setPosition(boyfriend->getMidpoint().x - 100, boyfriend->getMidpoint().y - 100);
    } else if (!mustHit && dad) {
        camFollow->setPosition(dad->getMidpoint().x + 150, dad->getMidpoint().y - 100);
    }
}