#pragma once

#include <flixel/FlxCamera.h>
#include <flixel/FlxObject.h>
#include "character/Character.h"
#include "song/Song.h"

class CameraManager {
public:
    CameraManager(flixel::FlxCamera* camGame, float defaultZoom);
    ~CameraManager();
    
    void initialize(Character* boyfriend, Character* dad);
    void update(float elapsed, int curSection, const SwagSong& song);
    
    flixel::FlxObject* getCamFollow() const { return camFollow; }
    float getDefaultZoom() const { return defaultCamZoom; }
    bool isCamZooming() const { return camZooming; }
    void setCamZooming(bool zooming) { camZooming = zooming; }
    
private:
    flixel::FlxCamera* camGame;
    flixel::FlxObject* camFollow;
    Character* boyfriend;
    Character* dad;
    float defaultCamZoom;
    bool camZooming;
    bool lastMustHit;
};