#pragma once

#include "../FunkinState.h"
#include "components/AtlasMenuItem.h"
#include <flixel/FlxSprite.h>
#include <flixel/FlxObject.h>
#include <vector>

class MainMenuState : public FunkinState {
public:
    MainMenuState();
    virtual ~MainMenuState();
    
    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;
    void beatHit() override;
    
private:
    flixel::FlxSprite* bg;
    flixel::FlxSprite* magenta;
    flixel::FlxObject* camFollow;
    
    std::vector<AtlasMenuItem*> menuItems;
    int selectedIndex;
    bool canSelect;
    bool exiting;
    
    void createMenuItem(const std::string& name, const std::string& atlas, std::function<void()> callback);
    void changeSelection(int change);
    void selectItem();
    void onMenuItemChange();
    void startExitState(std::function<void()> callback);
};