#include "Script.h"
#include "ScriptAPI.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cmath>

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

static std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

Script::Script(const std::string& scriptPath)
    : scriptPath(scriptPath)
    , loaded(false)
    , inFunction(false)
{
    size_t lastSlash = scriptPath.find_last_of("/\\");
    scriptName = (lastSlash != std::string::npos) ? scriptPath.substr(lastSlash + 1) : scriptPath;

    callbackFunctions[ScriptCallback::ON_CREATE] = "onCreate";
    callbackFunctions[ScriptCallback::ON_UPDATE] = "onUpdate";
    callbackFunctions[ScriptCallback::ON_DESTROY] = "onDestroy";
    callbackFunctions[ScriptCallback::ON_SONG_START] = "onSongStart";
    callbackFunctions[ScriptCallback::ON_SONG_END] = "onSongEnd";
    callbackFunctions[ScriptCallback::ON_COUNTDOWN_TICK] = "onCountdownTick";
    callbackFunctions[ScriptCallback::ON_COUNTDOWN_STARTED] = "onCountdownStarted";
    callbackFunctions[ScriptCallback::ON_BEAT_HIT] = "onBeatHit";
    callbackFunctions[ScriptCallback::ON_STEP_HIT] = "onStepHit";
    callbackFunctions[ScriptCallback::ON_NOTE_HIT] = "onNoteHit";
    callbackFunctions[ScriptCallback::ON_NOTE_MISS] = "onNoteMiss";
    callbackFunctions[ScriptCallback::ON_PAUSE] = "onPause";
    callbackFunctions[ScriptCallback::ON_RESUME] = "onResume";
    callbackFunctions[ScriptCallback::ON_SECTION_HIT] = "onSectionHit";
    callbackFunctions[ScriptCallback::ON_MOVE_CAMERA] = "onMoveCamera";
}

Script::~Script() {
}

bool Script::load() {
    std::ifstream file(scriptPath);
    if (!file.is_open()) {
        std::cerr << "Could not open script file: " << scriptPath << std::endl;
        return false;
    }

    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        lineNum++;
        parseLine(line, lineNum);
    }

    if (inFunction && !currentFunctionName.empty()) {
        Function func;
        func.name = currentFunctionName;
        func.commands = currentFunctionCommands;
        functions[currentFunctionName] = func;
        inFunction = false;
    }

    file.close();
    loaded = true;
    return true;
}

void Script::parseLine(const std::string& line, int lineNum) {
    std::string trimmed = trim(line);

    if (trimmed.empty() || trimmed[0] == '/' || trimmed[0] == '#') {
        return;
    }

    if (trimmed.find("function ") == 0) {
        if (inFunction && !currentFunctionName.empty()) {
            Function func;
            func.name = currentFunctionName;
            func.commands = currentFunctionCommands;
            functions[currentFunctionName] = func;
        }

        currentFunctionName = trim(trimmed.substr(9));
        currentFunctionCommands.clear();
        inFunction = true;
        return;
    }

    if (trimmed == "end function" || trimmed == "endfunction") {
        if (inFunction && !currentFunctionName.empty()) {
            Function func;
            func.name = currentFunctionName;
            func.commands = currentFunctionCommands;
            functions[currentFunctionName] = func;
            inFunction = false;
            currentFunctionName.clear();
            currentFunctionCommands.clear();
        }
        return;
    }

    if (inFunction) {
        Command cmd;

        size_t spacePos = trimmed.find(' ');
        if (spacePos != std::string::npos) {
            cmd.type = trimmed.substr(0, spacePos);
            std::string argsStr = trim(trimmed.substr(spacePos + 1));
            cmd.args = split(argsStr, ',');
        } else {
            cmd.type = trimmed;
        }

        cmd.lineNum = lineNum;
        currentFunctionCommands.push_back(cmd);
    }
}

std::vector<Script::Command> Script::parseBlock(std::vector<std::string>& lines, int& currentLine) {
    return {};
}

void Script::call(ScriptCallback callback, const std::vector<ScriptValue>& args) {
    auto it = callbackFunctions.find(callback);
    if (it != callbackFunctions.end()) {
        executeFunction(it->second, args);
    }
}

void Script::executeFunction(const std::string& funcName, const std::vector<ScriptValue>& args) {
    auto it = functions.find(funcName);
    if (it == functions.end()) {
        return;
    }

    if (funcName == "onNoteHit" && args.size() >= 2) {
        variables.set("noteData", args[0]);
        variables.set("rating", args[1]);
    } else if (funcName == "onNoteMiss" && args.size() >= 1) {
        variables.set("noteData", args[0]);
    } else if (funcName == "onBeatHit" && args.size() >= 1) {
        variables.set("curBeat", args[0]);
    } else if (funcName == "onStepHit" && args.size() >= 1) {
        variables.set("curStep", args[0]);
    } else if (funcName == "onCountdownTick" && args.size() >= 1) {
        variables.set("tick", args[0]);
    } else if (funcName == "onUpdate" && args.size() >= 1) {
        variables.set("elapsed", args[0]);
    }

    const Function& func = it->second;
    executeCommands(func.commands);
}

void Script::executeCommands(const std::vector<Command>& commands) {
    for (size_t i = 0; i < commands.size(); i++) {
        const Command& cmd = commands[i];

        if (cmd.type == "if") {
            if (cmd.args.empty()) continue;

            int elseIndex = -1;
            int endifIndex = -1;
            int nestLevel = 0;

            for (size_t j = i + 1; j < commands.size(); j++) {
                if (commands[j].type == "if") {
                    nestLevel++;
                } else if (commands[j].type == "else" && nestLevel == 0 && elseIndex == -1) {
                    elseIndex = j;
                } else if (commands[j].type == "endif") {
                    if (nestLevel == 0) {
                        endifIndex = j;
                        break;
                    }
                    nestLevel--;
                }
            }

            if (endifIndex == -1) {
                std::cerr << "Error: if without matching endif at line " << cmd.lineNum << std::endl;
                continue;
            }

            bool condition = evaluateCondition(cmd.args[0]);

            std::vector<Command> blockToExecute;
            if (condition) {
                int blockStart = i + 1;
                int blockEnd = (elseIndex != -1) ? elseIndex : endifIndex;
                for (int k = blockStart; k < blockEnd; k++) {
                    blockToExecute.push_back(commands[k]);
                }
            } else if (elseIndex != -1) {
                for (int k = elseIndex + 1; k < endifIndex; k++) {
                    blockToExecute.push_back(commands[k]);
                }
            }

            executeCommands(blockToExecute);

            i = endifIndex;
            continue;
        }

        if (cmd.type == "else" || cmd.type == "endif") {
            continue;
        }

        executeCommand(cmd);
    }
}

void Script::executeCommand(const Command& cmd) {
    const std::string& type = cmd.type;
    const auto& args = cmd.args;

    if (type == "set" || type == "var") {
        if (args.size() >= 2) {
            std::string varName = args[0];
            ScriptValue value = evaluateExpression(args[1]);
            variables.set(varName, value);
        }
        return;
    }

    if (ScriptAPI::hasFunction(type)) {
        std::vector<ScriptValue> scriptArgs;
        for (const auto& arg : args) {
            scriptArgs.push_back(evaluateExpression(arg));
        }
        ScriptAPI::callFunction(type, scriptArgs);
        return;
    }

    if (functions.find(type) != functions.end()) {
        executeFunction(type, {});
        return;
    }
}

ScriptValue Script::evaluateExpression(const std::string& expr) {
    std::string trimmedExpr = trim(expr);

    if (variables.has(trimmedExpr)) {
        return variables.get(trimmedExpr);
    }

    if (!trimmedExpr.empty() && (trimmedExpr[0] == '"' || trimmedExpr[0] == '\'')) {
        if (trimmedExpr.length() >= 2) {
            return trimmedExpr.substr(1, trimmedExpr.length() - 2);
        }
        return std::string("");
    }

    if (trimmedExpr == "true") return true;
    if (trimmedExpr == "false") return false;

    try {
        if (trimmedExpr.find('.') != std::string::npos) {
            return std::stof(trimmedExpr);
        } else {
            return std::stoi(trimmedExpr);
        }
    } catch (...) {
        if (ScriptAPI::hasFunction(trimmedExpr)) {
            return ScriptAPI::callFunction(trimmedExpr, {});
        }
        
        size_t spacePos = trimmedExpr.find(' ');
        if (spacePos != std::string::npos) {
            std::string funcName = trimmedExpr.substr(0, spacePos);
            if (ScriptAPI::hasFunction(funcName)) {
                std::string argsStr = trim(trimmedExpr.substr(spacePos + 1));
                std::vector<ScriptValue> funcArgs;
                std::vector<std::string> argTokens = split(argsStr, ' ');
                for (const auto& token : argTokens) {
                    funcArgs.push_back(evaluateExpression(token));
                }
                return ScriptAPI::callFunction(funcName, funcArgs);
            }
        }
        return trimmedExpr;
    }
}

float Script::evaluateNumeric(const std::string& expr) {
    ScriptValue val = evaluateExpression(expr);
    if (std::holds_alternative<int>(val)) {
        return static_cast<float>(std::get<int>(val));
    } else if (std::holds_alternative<float>(val)) {
        return std::get<float>(val);
    } else if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val) ? 1.0f : 0.0f;
    }
    return 0.0f;
}

bool Script::evaluateCondition(const std::string& condition) {
    std::string trimmed = trim(condition);

    std::vector<std::pair<std::string, int>> operators = {
        {"==", 0}, {"!=", 1}, {"<=", 2}, {">=", 3}, {"<", 4}, {">", 5}
    };

    for (const auto& op : operators) {
        size_t pos = trimmed.find(op.first);
        if (pos != std::string::npos) {
            std::string left = trim(trimmed.substr(0, pos));
            std::string right = trim(trimmed.substr(pos + op.first.length()));

            ScriptValue leftVal = evaluateExpression(left);
            ScriptValue rightVal = evaluateExpression(right);

            if (std::holds_alternative<int>(leftVal) || std::holds_alternative<float>(leftVal)) {
                float leftNum = evaluateNumeric(left);
                float rightNum = evaluateNumeric(right);

                switch (op.second) {
                    case 0: return leftNum == rightNum;
                    case 1: return leftNum != rightNum;
                    case 2: return leftNum <= rightNum;
                    case 3: return leftNum >= rightNum;
                    case 4: return leftNum < rightNum;
                    case 5: return leftNum > rightNum;
                }
            }

            if (std::holds_alternative<std::string>(leftVal) && std::holds_alternative<std::string>(rightVal)) {
                std::string leftStr = std::get<std::string>(leftVal);
                std::string rightStr = std::get<std::string>(rightVal);

                switch (op.second) {
                    case 0: return leftStr == rightStr;
                    case 1: return leftStr != rightStr;
                    default: return false;
                }
            }

            if (std::holds_alternative<bool>(leftVal) && std::holds_alternative<bool>(rightVal)) {
                bool leftBool = std::get<bool>(leftVal);
                bool rightBool = std::get<bool>(rightVal);

                switch (op.second) {
                    case 0: return leftBool == rightBool;
                    case 1: return leftBool != rightBool;
                    default: return false;
                }
            }

            return false;
        }
    }

    ScriptValue val = evaluateExpression(trimmed);

    if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val);
    } else if (std::holds_alternative<int>(val)) {
        return std::get<int>(val) != 0;
    } else if (std::holds_alternative<float>(val)) {
        return std::get<float>(val) != 0.0f;
    }
    return false;
}

ScriptValue Script::getVar(const std::string& name, const ScriptValue& defaultValue) const {
    return variables.get(name, defaultValue);
}

void Script::setVar(const std::string& name, const ScriptValue& value) {
    variables.set(name, value);
}
