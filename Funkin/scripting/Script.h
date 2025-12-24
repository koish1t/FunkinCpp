#pragma once

#include "ScriptTypes.h"
#include <string>
#include <vector>
#include <map>

class Script {
public:
    Script(const std::string& scriptPath);
    ~Script();

    bool load();
    void call(ScriptCallback callback, const std::vector<ScriptValue>& args = {});

    ScriptValue getVar(const std::string& name, const ScriptValue& defaultValue = 0) const;
    void setVar(const std::string& name, const ScriptValue& value);

    std::string getScriptName() const { return scriptName; }
    bool isLoaded() const { return loaded; }

private:
    struct Command;

    struct Command {
        std::string type;
        std::vector<std::string> args;
        int lineNum;
        std::vector<Command> ifBlock;
        std::vector<Command> elseBlock;
    };

    struct Function {
        std::string name;
        std::vector<Command> commands;
    };

    void parseLine(const std::string& line, int lineNum);
    std::vector<Command> parseBlock(std::vector<std::string>& lines, int& currentLine);
    void executeFunction(const std::string& funcName, const std::vector<ScriptValue>& args);
    void executeCommand(const Command& cmd);
    void executeCommands(const std::vector<Command>& commands);

    ScriptValue evaluateExpression(const std::string& expr);
    float evaluateNumeric(const std::string& expr);
    bool evaluateCondition(const std::string& condition);

    std::string scriptPath;
    std::string scriptName;
    bool loaded;

    ScriptVariables variables;
    std::map<ScriptCallback, std::string> callbackFunctions;
    std::map<std::string, Function> functions;

    bool inFunction;
    std::string currentFunctionName;
    std::vector<Command> currentFunctionCommands;
};
