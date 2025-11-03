#include "MainMenuState.h"
#include "TitleState.h"
#include "../play/PlayState.h"
#include <flixel/FlxG.h>
#include <flixel/FlxGame.h>
#include <flixel/sound/FlxSound.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

MainMenuState::MainMenuState() 
    : bg(nullptr)
    , magenta(nullptr)
    , camFollow(nullptr)
    , selectedIndex(0)
    , canSelect(true)
    , exiting(false)
{
}

MainMenuState::~MainMenuState() {
    destroy();
}

void MainMenuState::create() {
    if (!flixel::FlxG::camera) {
        flixel::FlxG::camera = new flixel::FlxCamera(0.0f, 0.0f, 0, 0, 1.0f);
    }
    
    bg = new flixel::FlxSprite(0, 0);
    bg->loadGraphic("assets/images/menuBG.png");
    bg->scrollFactor.x = 0.0f;
    bg->scrollFactor.y = 0.17f;
    bg->setScale(1.5f, 1.5f);
    bg->screenCenter();
    bg->updateHitbox();
    bg->camera = flixel::FlxG::camera;
    
    camFollow = new flixel::FlxObject(0, 0, 1, 1);
    
    magenta = new flixel::FlxSprite(0, 0);
    magenta->loadGraphic("assets/images/menuBGMagenta.png");
    magenta->scrollFactor.x = bg->scrollFactor.x;
    magenta->scrollFactor.y = bg->scrollFactor.y;
    magenta->setScale(1.5f, 1.5f);
    magenta->x = bg->x;
    magenta->y = bg->y;
    magenta->visible = false;
    bg->screenCenter();
    magenta->updateHitbox();
    magenta->camera = flixel::FlxG::camera;
    
    createMenuItem("storymode", "assets/images/mainmenu/storymode", []() {
        std::cout << "Story Mode selected (not implemented)" << std::endl;
    });
    
    createMenuItem("freeplay", "assets/images/mainmenu/freeplay", []() {
        std::cout << "Freeplay selected (not implemented)" << std::endl;
    });
    
    createMenuItem("options", "assets/images/mainmenu/options", []() {
        std::cout << "Options selected (not implemented)" << std::endl;
    });
    
    createMenuItem("credits", "assets/images/mainmenu/credits", []() {
        std::cout << "Credits selected (not implemented)" << std::endl;
    });
    
    int spacing = 160;
    int top = (flixel::FlxG::height - (spacing * (static_cast<int>(menuItems.size()) - 1))) / 2;
    for (size_t i = 0; i < menuItems.size(); i++) {
        menuItems[i]->x = flixel::FlxG::width / 2.0f;
        menuItems[i]->y = static_cast<float>(top + spacing * i);
        menuItems[i]->scrollFactor.x = 0.0f;
        menuItems[i]->scrollFactor.y = 0.4f;
        menuItems[i]->camera = flixel::FlxG::camera;
        
        if (menuItems[i]->centered && menuItems[i]->frames) {
            menuItems[i]->x -= menuItems[i]->width / 2.0f;
        }
    }
    
    if (flixel::FlxG::camera && camFollow) {
        flixel::FlxG::camera->target = camFollow;
        flixel::FlxG::camera->followLerp = 0.06f;
        flixel::FlxG::camera->deadzone = nullptr;
        flixel::FlxG::camera->focusOn(flixel::FlxPoint(camFollow->x, camFollow->y));
    }
    
    onMenuItemChange();
}

void MainMenuState::update(float elapsed) {
    FunkinState::update(elapsed);
    
    if (bg) bg->update(elapsed);
    if (magenta) magenta->update(elapsed);
    if (camFollow) camFollow->update(elapsed);
    
    for (auto* item : menuItems) {
        if (item) {
            item->update(elapsed);
            
            if (item->centered) {
                item->x = flixel::FlxG::width / 2.0f - item->width / 2.0f;
            }
        }
    }
    
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(menuItems.size()) && camFollow) {
        camFollow->x = menuItems[selectedIndex]->x + menuItems[selectedIndex]->width / 2.0f;
        camFollow->y = menuItems[selectedIndex]->y + menuItems[selectedIndex]->height / 2.0f;
    }
    
    if (Mix_PlayingMusic() && Mix_VolumeMusic(-1) < static_cast<int>(0.8f * MIX_MAX_VOLUME)) {
        int currentVolume = Mix_VolumeMusic(-1);
        Mix_VolumeMusic(currentVolume + static_cast<int>(0.5f * elapsed * MIX_MAX_VOLUME));
    }
    
    if (canSelect && !exiting) {
        if (flixel::FlxG::keys.keys[SDL_SCANCODE_UP].justPressed()) {
            changeSelection(-1);
        }
        if (flixel::FlxG::keys.keys[SDL_SCANCODE_DOWN].justPressed()) {
            changeSelection(1);
        }
        
        if (flixel::FlxG::keys.keys[SDL_SCANCODE_RETURN].justPressed()) {
            selectItem();
        }
        
        if (flixel::FlxG::keys.keys[SDL_SCANCODE_ESCAPE].justPressed()) {
            flixel::FlxG::sound.play("assets/sounds/cancelMenu.ogg");
            startExitState([]() {
                flixel::FlxG::game->switchState(new TitleState());
            });
        }
    }
}

void MainMenuState::draw() {
    if (bg) bg->draw();
    if (magenta && magenta->visible) magenta->draw();
    
    for (auto* item : menuItems) {
        if (item) {
            item->draw();
        }
    }
}

void MainMenuState::destroy() {
    for (auto* item : menuItems) {
        if (item) {
            delete item;
        }
    }
    menuItems.clear();
    
    if (bg) {
        delete bg;
        bg = nullptr;
    }
    if (magenta) {
        delete magenta;
        magenta = nullptr;
    }
    if (camFollow) {
        delete camFollow;
        camFollow = nullptr;
    }
    
    FunkinState::destroy();
}

void MainMenuState::beatHit() {
    if (magenta) {
        magenta->visible = !magenta->visible;
    }
}

void MainMenuState::createMenuItem(const std::string& name, const std::string& atlas, std::function<void()> callback) {
    AtlasMenuItem* item = new AtlasMenuItem(name, atlas, callback);
    item->ID = static_cast<int>(menuItems.size());
    item->centered = true;
    item->changeAnim("idle");
    menuItems.push_back(item);
}

void MainMenuState::changeSelection(int change) {
    flixel::FlxG::sound.play("assets/sounds/scrollMenu.ogg");
    
    selectedIndex += change;
    
    if (selectedIndex < 0) {
        selectedIndex = static_cast<int>(menuItems.size()) - 1;
    }
    if (selectedIndex >= static_cast<int>(menuItems.size())) {
        selectedIndex = 0;
    }
    
    onMenuItemChange();
}

void MainMenuState::selectItem() {
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(menuItems.size())) {
        flixel::FlxG::sound.play("assets/sounds/confirmMenu.ogg");
        
        canSelect = false;
        
        if (menuItems[selectedIndex]->name == "freeplay") {
            magenta->visible = true;
        }
        
        if (menuItems[selectedIndex]->callback) {
            menuItems[selectedIndex]->callback();
        }
    }
}

void MainMenuState::onMenuItemChange() {
    for (size_t i = 0; i < menuItems.size(); i++) {
        if (static_cast<int>(i) == selectedIndex) {
            menuItems[i]->changeAnim("selected");
        } else {
            menuItems[i]->changeAnim("idle");
        }
        
        if (menuItems[i]->centered) {
            menuItems[i]->x = flixel::FlxG::width / 2.0f - menuItems[i]->width / 2.0f;
        }
    }
    
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(menuItems.size()) && camFollow) {
        camFollow->x = menuItems[selectedIndex]->x + menuItems[selectedIndex]->width / 2.0f;
        camFollow->y = menuItems[selectedIndex]->y + menuItems[selectedIndex]->height / 2.0f;
    }
}

void MainMenuState::startExitState(std::function<void()> callback) {
    exiting = true;
    canSelect = false;
    
    if (callback) {
        callback();
    }
}
