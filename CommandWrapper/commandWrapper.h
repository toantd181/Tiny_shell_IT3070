// CommandWrapper.h
#ifndef COMMANDWRAPPER_H
#define COMMANDWRAPPER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

// Định nghĩa alias cho hàm lệnh
using CommandFunction = std::function<void(const std::vector<std::string>&)>;

class CommandWrapper {
private:
    static std::unordered_map<std::string, CommandFunction> internalCommands;

public:
    static void initializeCommands();
    static bool executeCommand(const std::vector<std::string>& args, bool isBackground);
    static bool isInternalCommand(const std::string& command);
    static std::vector<std::string> getInternalCommands();
    static void addCommand(const std::string& name, CommandFunction function);
    static void removeCommand(const std::string& name);
};

#endif // COMMANDWRAPPER_H
