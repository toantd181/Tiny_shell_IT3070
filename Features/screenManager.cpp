#include "screenManager.h"

const std::unordered_set<std::string> ScreenManager::supportedCommands = {
    "write_to_screen",
    "read_file",
    "clear_terminal"
};

const std::unordered_set<std::string>& ScreenManager::getSupportedCommands() {
    return supportedCommands;
}