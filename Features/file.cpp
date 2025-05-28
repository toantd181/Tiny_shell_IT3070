#include "../Features/file.h"

const std::unordered_set<std::string> FileManager::supportedCommands = {
    "create_file",
    "delete_file",
    "copy_file",
    "move_file",
    "list_files"
};

const std::unordered_set<std::string>& FileManager::getSupportedCommands() {
    return supportedCommands;
}   