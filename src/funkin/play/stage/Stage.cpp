#include "Stage.h"
#include <flixel/graphics/frames/FlxAtlasFrames.h>
#include <algorithm>

Stage::Stage(const std::string& stageName) {
    curStage = stageName;
    defaultZoom = 1.05f;
    buildStage();
}

Stage::~Stage() {
    for (auto sprite : sprites) {
        if (sprite) {
            delete sprite;
        }
    }
    sprites.clear();
}

bool Stage::isPixelStage() const {
    return curStage == "school" || curStage == "schoolEvil";
}

std::string Stage::getStageFromSong(const std::string& songName) {
    std::string lowerSong = songName;
    std::transform(lowerSong.begin(), lowerSong.end(), lowerSong.begin(), ::tolower);
    
    if (lowerSong == "spookeez" || lowerSong == "monster" || lowerSong == "south") {
        return "spooky";
    }
    else if (lowerSong == "pico" || lowerSong == "blammed" || lowerSong == "philly") {
        return "philly";
    }
    else if (lowerSong == "milf" || lowerSong == "satin-panties" || lowerSong == "high") {
        return "limo";
    }
    else if (lowerSong == "cocoa" || lowerSong == "eggnog") {
        return "mall";
    }
    else if (lowerSong == "winter-horrorland") {
        return "mallEvil";
    }
    else if (lowerSong == "senpai" || lowerSong == "roses") {
        return "school";
    }
    else if (lowerSong == "thorns") {
        return "schoolEvil";
    }
    return "stage";
}

void Stage::buildStage() {
    if (curStage == "stage") {
        buildDefaultStage();
    }
    else if (curStage == "spooky") {
        buildSpookyStage();
    }
    else if (curStage == "philly") {
        buildPhillyStage();
    }
    else if (curStage == "limo") {
        buildLimoStage();
    }
    else if (curStage == "mall") {
        buildMallStage();
    }
    else if (curStage == "mallEvil") {
        buildMallEvilStage();
    }
    else if (curStage == "school") {
        buildSchoolStage();
    }
    else if (curStage == "schoolEvil") {
        buildSchoolEvilStage();
    }
}

flixel::FlxSprite* Stage::createSprite(float x, float y, const std::string& imagePath) {
    flixel::FlxSprite* sprite = new flixel::FlxSprite(x, y);
    sprite->loadGraphic(imagePath);
    sprites.push_back(sprite);
    return sprite;
}

flixel::FlxSprite* Stage::createAnimatedSprite(float x, float y, const std::string& imagePath, const std::string& xmlPath) {
    flixel::FlxSprite* sprite = new flixel::FlxSprite(x, y);
    auto frames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow(imagePath, xmlPath);
    sprite->frames = frames;
    sprite->texture = frames->texture;
    sprite->ownsTexture = false;
    sprite->animation = new flixel::animation::FlxAnimationController();
    sprites.push_back(sprite);
    return sprite;
}

void Stage::buildDefaultStage() {
    defaultZoom = 1.05f;
    
    auto bg = createSprite(-600, -200, "assets/images/stageback.png");
    bg->scrollFactor.set(0.9f, 0.9f);
    bg->active = false;
    
    auto stageFront = createSprite(-650, 600, "assets/images/stagefront.png");
    stageFront->setScale(1.1f, 1.1f);
    stageFront->updateHitbox();
    stageFront->scrollFactor.set(0.9f, 0.9f);
    stageFront->active = false;
    
    auto stageCurtains = createSprite(-500, -300, "assets/images/stagecurtains.png");
    stageCurtains->setScale(0.9f, 0.9f);
    stageCurtains->updateHitbox();
    stageCurtains->scrollFactor.set(1.3f, 1.3f);
    stageCurtains->active = false;
}

void Stage::buildSpookyStage() {
    defaultZoom = 1.05f;
    
    auto halloweenBG = createAnimatedSprite(-200, -100, "assets/images/halloween_bg.png", "assets/images/halloween_bg.xml");
    auto frames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow("assets/images/halloween_bg.png", "assets/images/halloween_bg.xml");
    
    auto idleFrames = frames->getFramesByPrefix("halloweem bg0");
    if (!idleFrames.empty()) {
        halloweenBG->animation->addByPrefix("idle", idleFrames, 24, true);
        halloweenBG->animation->play("idle");
    }
}

void Stage::buildPhillyStage() {
    defaultZoom = 1.05f;
    
    auto bg = createSprite(-100, 0, "assets/images/philly/sky.png");
    bg->scrollFactor.set(0.1f, 0.1f);
    
    auto city = createSprite(-10, 0, "assets/images/philly/city.png");
    city->scrollFactor.set(0.3f, 0.3f);
    city->setScale(0.85f, 0.85f);
    city->updateHitbox();
    
    for (int i = 0; i < 5; i++) {
        auto light = createSprite(city->x, city->y, "assets/images/philly/win" + std::to_string(i) + ".png");
        light->scrollFactor.set(0.3f, 0.3f);
        light->visible = false;
        light->setScale(0.85f, 0.85f);
        light->updateHitbox();
    }
    
    auto streetBehind = createSprite(-40, 50, "assets/images/philly/behindTrain.png");
    
    auto phillyTrain = createSprite(2000, 360, "assets/images/philly/train.png");
    
    auto street = createSprite(-40, 50, "assets/images/philly/street.png");
}

void Stage::buildLimoStage() {
    defaultZoom = 0.9f;
    
    auto skyBG = createSprite(-120, -50, "assets/images/limo/limoSunset.png");
    skyBG->scrollFactor.set(0.1f, 0.1f);
    
    auto bgLimo = createAnimatedSprite(-200, 480, "assets/images/limo/bgLimo.png", "assets/images/limo/bgLimo.xml");
    auto frames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow("assets/images/limo/bgLimo.png", "assets/images/limo/bgLimo.xml");
    auto driveFrames = frames->getFramesByPrefix("background limo pink");
    if (!driveFrames.empty()) {
        bgLimo->animation->addByPrefix("drive", driveFrames, 24, true);
        bgLimo->animation->play("drive");
    }
    bgLimo->scrollFactor.set(0.4f, 0.4f);
    
    auto limo = createAnimatedSprite(-120, 550, "assets/images/limo/limoDrive.png", "assets/images/limo/limoDrive.xml");
    auto limoFrames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow("assets/images/limo/limoDrive.png", "assets/images/limo/limoDrive.xml");
    auto limoStageFrames = limoFrames->getFramesByPrefix("Limo stage");
    if (!limoStageFrames.empty()) {
        limo->animation->addByPrefix("drive", limoStageFrames, 24, true);
        limo->animation->play("drive");
    }
}

void Stage::buildMallStage() {
    defaultZoom = 0.8f;
    
    auto bg = createSprite(-1000, -500, "assets/images/christmas/bgWalls.png");
    bg->scrollFactor.set(0.2f, 0.2f);
    bg->active = false;
    bg->setScale(0.8f, 0.8f);
    bg->updateHitbox();
    
    auto upperBoppers = createAnimatedSprite(-240, -90, "assets/images/christmas/upperBop.png", "assets/images/christmas/upperBop.xml");
    auto frames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow("assets/images/christmas/upperBop.png", "assets/images/christmas/upperBop.xml");
    auto bopFrames = frames->getFramesByPrefix("Upper Crowd Bob");
    if (!bopFrames.empty()) {
        upperBoppers->animation->addByPrefix("bop", bopFrames, 24, false);
    }
    upperBoppers->scrollFactor.set(0.33f, 0.33f);
    upperBoppers->setScale(0.85f, 0.85f);
    upperBoppers->updateHitbox();
    
    auto bgEscalator = createSprite(-1100, -600, "assets/images/christmas/bgEscalator.png");
    bgEscalator->scrollFactor.set(0.3f, 0.3f);
    bgEscalator->active = false;
    bgEscalator->setScale(0.9f, 0.9f);
    bgEscalator->updateHitbox();
    
    auto tree = createSprite(370, -250, "assets/images/christmas/christmasTree.png");
    tree->scrollFactor.set(0.4f, 0.4f);
    
    auto bottomBoppers = createAnimatedSprite(-300, 140, "assets/images/christmas/bottomBop.png", "assets/images/christmas/bottomBop.xml");
    auto bottomFrames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow("assets/images/christmas/bottomBop.png", "assets/images/christmas/bottomBop.xml");
    auto bottomBopFrames = bottomFrames->getFramesByPrefix("Bottom Level Boppers");
    if (!bottomBopFrames.empty()) {
        bottomBoppers->animation->addByPrefix("bop", bottomBopFrames, 24, false);
    }
    bottomBoppers->scrollFactor.set(0.9f, 0.9f);
    bottomBoppers->updateHitbox();
    
    auto fgSnow = createSprite(-600, 700, "assets/images/christmas/fgSnow.png");
    fgSnow->active = false;
    
    auto santa = createAnimatedSprite(-840, 150, "assets/images/christmas/santa.png", "assets/images/christmas/santa.xml");
    auto santaFrames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow("assets/images/christmas/santa.png", "assets/images/christmas/santa.xml");
    auto santaIdleFrames = santaFrames->getFramesByPrefix("santa idle in fear");
    if (!santaIdleFrames.empty()) {
        santa->animation->addByPrefix("idle", santaIdleFrames, 24, false);
    }
}

void Stage::buildMallEvilStage() {
    defaultZoom = 1.05f;
    
    auto bg = createSprite(-400, -500, "assets/images/christmas/evilBG.png");
    bg->scrollFactor.set(0.2f, 0.2f);
    bg->active = false;
    bg->setScale(0.8f, 0.8f);
    bg->updateHitbox();
    
    auto evilTree = createSprite(300, -300, "assets/images/christmas/evilTree.png");
    evilTree->scrollFactor.set(0.2f, 0.2f);
    
    auto evilSnow = createSprite(-200, 700, "assets/images/christmas/evilSnow.png");
}

void Stage::buildSchoolStage() {
    defaultZoom = 1.05f;
    
    auto bgSky = createSprite(0, 0, "assets/images/weeb/weebSky.png");
    bgSky->scrollFactor.set(0.1f, 0.1f);
    
    int repositionShit = -200;
    
    auto bgSchool = createSprite(repositionShit, 0, "assets/images/weeb/weebSchool.png");
    bgSchool->scrollFactor.set(0.6f, 0.9f);
    
    auto bgStreet = createSprite(repositionShit, 0, "assets/images/weeb/weebStreet.png");
    bgStreet->scrollFactor.set(0.95f, 0.95f);
    
    auto fgTrees = createSprite(repositionShit + 170, 130, "assets/images/weeb/weebTreesBack.png");
    fgTrees->scrollFactor.set(0.9f, 0.9f);
    
    auto treeLeaves = createAnimatedSprite(repositionShit, -40, "assets/images/weeb/petals.png", "assets/images/weeb/petals.xml");
    auto frames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow("assets/images/weeb/petals.png", "assets/images/weeb/petals.xml");
    auto leavesFrames = frames->getFramesByPrefix("PETALS ALL");
    if (!leavesFrames.empty()) {
        treeLeaves->animation->addByPrefix("leaves", leavesFrames, 24, true);
        treeLeaves->animation->play("leaves");
    }
    treeLeaves->scrollFactor.set(0.85f, 0.85f);
    
    int widShit = bgSky->width * 6;
    
    bgSky->setScale(widShit / static_cast<float>(bgSky->width), widShit / static_cast<float>(bgSky->width));
    bgSchool->setScale(widShit / static_cast<float>(bgSchool->width), widShit / static_cast<float>(bgSchool->width));
    bgStreet->setScale(widShit / static_cast<float>(bgStreet->width), widShit / static_cast<float>(bgStreet->width));
    fgTrees->setScale((widShit * 0.8f) / static_cast<float>(fgTrees->width), (widShit * 0.8f) / static_cast<float>(fgTrees->width));
    treeLeaves->setScale(widShit / static_cast<float>(treeLeaves->width), widShit / static_cast<float>(treeLeaves->width));
    
    fgTrees->updateHitbox();
    bgSky->updateHitbox();
    bgSchool->updateHitbox();
    bgStreet->updateHitbox();
    treeLeaves->updateHitbox();
}

void Stage::buildSchoolEvilStage() {
    defaultZoom = 1.05f;
    
    float posX = 400;
    float posY = 200;
    
    auto bg = createAnimatedSprite(posX, posY, "assets/images/weeb/animatedEvilSchool.png", "assets/images/weeb/animatedEvilSchool.xml");
    auto frames = flixel::graphics::frames::FlxAtlasFrames::fromSparrow("assets/images/weeb/animatedEvilSchool.png", "assets/images/weeb/animatedEvilSchool.xml");
    auto idleFrames = frames->getFramesByPrefix("background 2");
    if (!idleFrames.empty()) {
        bg->animation->addByPrefix("idle", idleFrames, 24, true);
        bg->animation->play("idle");
    }
    bg->scrollFactor.set(0.8f, 0.9f);
    bg->setScale(6, 6);
}

