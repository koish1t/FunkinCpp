#pragma once

#include <flixel/FlxSprite.h>

class CheckBox : public flixel::FlxSprite {
public:
    CheckBox(float x, float y, bool checked = false);
    virtual ~CheckBox();
    
    void setChecked(bool checked);
    bool isChecked() const { return checked; }
    
    void update(float elapsed) override;
    void draw() override;

private:
    bool checked;
};