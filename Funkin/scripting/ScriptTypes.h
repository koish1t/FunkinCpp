#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <functional>

class Script;
class ScriptManager;

using ScriptValue = std::variant<int, float, bool, std::string>;

enum class ScriptCallback {
    ON_CREATE,
    ON_UPDATE,
    ON_DESTROY,

    ON_SONG_START,
    ON_SONG_END,
    ON_COUNTDOWN_TICK,
    ON_COUNTDOWN_STARTED,

    ON_BEAT_HIT,
    ON_STEP_HIT,

    ON_NOTE_HIT,
    ON_NOTE_MISS,
    ON_GHOST_TAP,

    ON_PAUSE,
    ON_RESUME,
    ON_GAME_OVER,
    ON_GAME_OVER_START,
    ON_GAME_OVER_CONFIRM,

    ON_SECTION_HIT,
    ON_MOVE_CAMERA,

    ON_CHARACTER_ANIM,
    ON_DAD_HIT,
    ON_BOYFRIEND_HIT,

    ON_CAMERA_ZOOM,

    ON_TIMER_COMPLETE,
    ON_TWEEN_COMPLETE,

    ON_UPDATE_SCORE,
    ON_UPDATE_HEALTH,

    ON_START_DIALOGUE,
    ON_NEXT_DIALOGUE,
    ON_SKIP_DIALOGUE,
    ON_END_DIALOGUE,

    ON_CUSTOM_EVENT
};

using ScriptFunction = std::function<ScriptValue(const std::vector<ScriptValue>&)>;

using ScriptCallbackFunc = std::function<void(const std::vector<ScriptValue>&)>;

struct ScriptVariables {
    std::map<std::string, ScriptValue> vars;

    void set(const std::string& name, const ScriptValue& value) {
        vars[name] = value;
    }

    ScriptValue get(const std::string& name, const ScriptValue& defaultValue = 0) const {
        auto it = vars.find(name);
        if (it != vars.end()) {
            return it->second;
        }
        return defaultValue;
    }

    bool has(const std::string& name) const {
        return vars.find(name) != vars.end();
    }
};
