#pragma once

#include "stage/Stage.h"
#include "character/Character.h"
#include "notes/Strumline.h"
#include "notes/NoteManager.h"
#include "components/HealthBar.h"
#include "components/PopUpStuff.h"
#include "Countdown.h"
#include <flixel/text/FlxText.h>
#include <flixel/FlxSubState.h>

class PlayStateRenderer {
public:
    void draw(Stage* stage, Character* gf, Character* dad, Character* boyfriend,
              Strumline* opponentStrumline, Strumline* playerStrumline,
              NoteManager* noteManager, HealthBar* healthBar,
              flixel::FlxText* scoreText, PopUpStuff* popUpStuff,
              Countdown* countdown, flixel::FlxSubState* subState);
};