#include "../Features/directory.h"

const std::unordered_set<std::string> DirectoryManager::supportedCommands = {
    "copy",
    "make_dir",
    "delete_dir",
    "list_dir_tree",
    "move_dir",
    "change_dir"
};

const std::unordered_set<std::string>& DirectoryManager::getSupportedCommands() {
    return supportedCommands;
}
