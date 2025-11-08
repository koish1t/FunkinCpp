#pragma once

#include "Script.h"
#include "ScriptTypes.h"
#include <vector>
#include <memory>
#include <string>

class ScriptManager {
public:
    static ScriptManager* getInstance();

    void loadScript(const std::string& scriptPath);
    void loadScriptsFromDirectory(const std::string& directory);
    void loadSongScripts(const std::string& songName);
    void loadCharacterScripts(const std::vector<std::string>& characterNames);
    void loadStageScripts(const std::string& stageName);
    void clear();

    void callAll(ScriptCallback callback, const std::vector<ScriptValue>& args = {});
    void drawScriptObjects();
    void updateScriptObjects(float elapsed);

    void setGlobalVar(const std::string& name, const ScriptValue& value);
    ScriptValue getGlobalVar(const std::string& name, const ScriptValue& defaultValue = 0) const;

    size_t getScriptCount() const { return scripts.size(); }

private:
    ScriptManager();
    ~ScriptManager();

    static ScriptManager* instance;
    std::vector<std::unique_ptr<Script>> scripts;
    ScriptVariables globalVars;
};
