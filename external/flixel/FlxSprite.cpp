#include "FlxSprite.h"
#include "FlxG.h"

namespace flixel {

FlxSprite::FlxSprite(float x, float y) 
    : FlxObject(x, y, 0, 0)
{
}

FlxSprite::~FlxSprite() {
    destroy();
}

void FlxSprite::loadGraphic(const std::string& path) {
    if (texture && ownsTexture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    
    try {
        texture = FlxG::loadTexture(path);
        ownsTexture = true;
    } catch (const std::exception&) {
        try {
            texture = FlxG::loadTexture("assets/images/logo/default.png");
            ownsTexture = true;
            FlxG::log.warn("Sprite not found: " + path + ", loaded fallback image instead.");
        } catch (const std::exception& e) {
            FlxG::log.error("Failed to load both sprite and fallback image: " + std::string(e.what()));
            texture = nullptr;
            ownsTexture = false;
            return;
        }
    }
    
    SDL_QueryTexture(texture, nullptr, nullptr, &sourceRect.w, &sourceRect.h);
    destRect.w = sourceRect.w;
    destRect.h = sourceRect.h;
    
    width = static_cast<float>(sourceRect.w);
    height = static_cast<float>(sourceRect.h);
    frameWidth = sourceRect.w;
    frameHeight = sourceRect.h;
    centerOrigin();
}

void FlxSprite::loadGraphic(SDL_Texture* newTexture) {
    if (texture && ownsTexture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    
    if (newTexture) {
        texture = newTexture;
        ownsTexture = true;
    } else {
        try {
            texture = FlxG::loadTexture("assets/images/logo/default.png");
            ownsTexture = true;
            FlxG::log.warn("Null texture provided, loaded fallback image instead.");
        } catch (const std::exception& e) {
            FlxG::log.error("Failed to load fallback image: " + std::string(e.what()));
            texture = nullptr;
            ownsTexture = false;
            return;
        }
    }
    
    SDL_QueryTexture(texture, nullptr, nullptr, &sourceRect.w, &sourceRect.h);
    destRect.w = sourceRect.w;
    destRect.h = sourceRect.h;
    
    width = static_cast<float>(sourceRect.w);
    height = static_cast<float>(sourceRect.h);
    frameWidth = sourceRect.w;
    frameHeight = sourceRect.h;
    centerOrigin();
}

void FlxSprite::updateHitbox() {
    width = std::abs(scaleX) * static_cast<float>(frameWidth);
    height = std::abs(scaleY) * static_cast<float>(frameHeight);
    offsetX = -0.5f * (width - static_cast<float>(frameWidth));
    offsetY = -0.5f * (height - static_cast<float>(frameHeight));
    centerOrigin();
}

void FlxSprite::centerOffsets(bool adjustPosition) {
    offsetX = (static_cast<float>(frameWidth) - width) * 0.5f;
    offsetY = (static_cast<float>(frameHeight) - height) * 0.5f;
    
    if (adjustPosition) {
        x += offsetX;
        y += offsetY;
    }
}

void FlxSprite::centerOrigin() {
    originX = static_cast<float>(frameWidth) * 0.5f;
    originY = static_cast<float>(frameHeight) * 0.5f;
}

void FlxSprite::draw() {
    if (!texture || !visible) return;

    const SDL_Rect* srcRect = &sourceRect;
    SDL_Rect frameRect;
    float frameOffsetX = 0.0f;
    float frameOffsetY = 0.0f;
    
    if (frames && animation) {
        int frameIdx = animation->getCurrentFrame();
        if (frameIdx >= 0 && frameIdx < static_cast<int>(frames->frames.size())) {
            const auto& frame = frames->frames[frameIdx];
            frameRect = frame.rect;
            srcRect = &frameRect;
            frameOffsetX = static_cast<float>(frame.sourceSize.x) * scaleX;
            frameOffsetY = static_cast<float>(frame.sourceSize.y) * scaleY;
        }
    }

    destRect.x = static_cast<int>(x - offsetX + frameOffsetX);
    destRect.y = static_cast<int>(y - offsetY + frameOffsetY);
    destRect.w = static_cast<int>(srcRect->w * scaleX);
    destRect.h = static_cast<int>(srcRect->h * scaleY);

    if (color != 0xFFFFFFFF) {
        const Uint8 r = static_cast<Uint8>((color >> 16) & 0xFF);
        const Uint8 g = static_cast<Uint8>((color >> 8) & 0xFF);
        const Uint8 b = static_cast<Uint8>(color & 0xFF);
        SDL_SetTextureColorMod(texture, r, g, b);
    }

    if (alpha < 1.0f) {
        SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(alpha * 255.0f));
    }

    if (angle == 0.0f) {
        SDL_RenderCopy(FlxG::renderer, texture, srcRect, &destRect);
    } else {
        SDL_RenderCopyEx(FlxG::renderer, texture, srcRect, &destRect, angle, nullptr, SDL_FLIP_NONE);
    }
}

void FlxSprite::destroy() {
    if (texture && ownsTexture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
        ownsTexture = false;
    }
    FlxObject::destroy();
}
}