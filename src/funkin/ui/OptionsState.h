#pragma once

#include "../FunkinState.h"
#include "components/Alphabet.h"
#include <flixel/FlxSprite.h>
#include <flixel/FlxCamera.h>
#include <vector>
#include <string>
#include <functional>
#include <map>

class CheckBox;

enum class SubMenu {
    None,
    Preferences,
    Controls
};

class OptionsState : public FunkinState {
public:
    OptionsState();
    virtual ~OptionsState();
    
    void create() override;
    void update(float elapsed) override;
    void draw() override;
    void destroy() override;

private:
    flixel::FlxSprite* bg;
    flixel::FlxCamera* controlsCamera;
    float camFollowY;
    std::vector<Alphabet*> menuItems;
    int selectedIndex;
    SubMenu currentSubMenu;
    
    std::vector<Alphabet*> prefLabels;
    std::vector<CheckBox*> checkBoxes;
    std::map<std::string, bool> preferences;
    int prefSelectedIndex;
    
    std::vector<Alphabet*> controlHeaders;
    std::vector<Alphabet*> controlLabels;
    std::vector<Alphabet*> controlBindLabels;
    std::vector<Alphabet*> controlBindLabelsAlt;
    Alphabet* deviceLabel;
    int controlSelectedIndex;
    int controlColumnIndex;
    bool awaitingInput;
    bool isGamepadMode;
    bool selectingDevice;
    
    void changeSelection(int change);
    void selectItem();
    void goBack();
    
    void enterPreferencesMenu();
    void exitPreferencesMenu();
    void togglePreference();
    
    void enterControlsMenu();
    void exitControlsMenu();
    void startRebinding();
    void captureInput();
    
    void loadPreferences();
    void savePreferences();
    void updateControlBindings();
    
    void drawMainMenu();
    void drawPreferencesMenu();
    void drawControlsMenu();
};