// CommandWrapper.h
#ifndef COMMANDWRAPPER_H
#define COMMANDWRAPPER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

// Định nghĩa alias cho hàm lệnh
using CommandFunction = std::function<void(const std::vector<std::string>&)>;

namespace ProcessManager {
    void listProcesses();
    void killProcess(const std::vector<std::string>& args);
    void stopProcess(const std::vector<std::string>& args);
    void resumeProcess(const std::vector<std::string>& args);
}

namespace EnvironmentManager {
    void showPath();
    void addPath(const std::vector<std::string>& args);
}


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
