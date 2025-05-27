// CommandWrapper.cpp
#include "commandWrapper.h"
#include "../Features/features.h"
#include "../Processes/processes.h"
#include "../Features/alias.h"
#include "../Builtin/builtins.h"
#include <iostream>
#include <thread>
#include <vector>
#include <exception>

using namespace std;

// Manager instances
static DirectoryManager directoryManager;
static FileManager fileManager;
static ScreenManager screenManager;
static AliasManager aliasManager;

// Background Process Manager (local to this file)
class BackgroundManager {
private:
    static std::vector<std::thread> backgroundThreads;
    static int nextJobId;

public:
    static void executeInternalCommandBackground(const std::vector<std::string>& args, CommandFunction command) {
        int jobId = ++nextJobId;
        std::cout << "[Background Job " << jobId << "] Started" << std::endl;

        backgroundThreads.emplace_back([args, command, jobId]() {
            try {
                command(args);
                std::cout << "[Background Job " << jobId << "] Completed" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[Background Job " << jobId << "] Error: " << e.what() << std::endl;
            }
        });

        backgroundThreads.back().detach();
    }

    static void cleanupFinishedThreads() {
        // Placeholder for future thread cleanup logic
    }
};

std::vector<std::thread> BackgroundManager::backgroundThreads;
int BackgroundManager::nextJobId = 0;

unordered_map<std::string, CommandFunction> CommandWrapper::internalCommands;

void CommandWrapper::initializeCommands() {
    internalCommands = {
        {"copy",           [&](auto& a) { directoryManager.copyDirectory(a); }},
        {"create_dir",     [&](auto& a) { directoryManager.createDirectory(a); }},
        {"delete_dir",     [&](auto& a) { directoryManager.deleteDirectory(a); }},
        {"list_dir_tree",  [&](auto& a) { directoryManager.listDirectoryTree(a); }},
        {"move_dir",       [&](auto& a) { directoryManager.moveDirectory(a); }},
        {"change_dir",     [&](auto& a) { directoryManager.changeDirectory(a); }},

        {"create_file",    [&](auto& a) { fileManager.createFile(a); }},
        {"delete_file",    [&](auto& a) { fileManager.deleteFile(a); }},
        {"copy_file",      [&](auto& a) { fileManager.copyFile(a); }},
        {"move_file",      [&](auto& a) { fileManager.moveFile(a); }},
        {"list_files",     [&](auto& a) { fileManager.listFiles(a); }},

        {"write_to_screen",  [&](auto& a) { screenManager.writeToScreen(a); }},
        {"read_file",        [&](auto& a) { screenManager.readFile(a); }},
        {"clear_terminal",   [&](auto&)   { screenManager.clearTerminal(); }},

        {"sliip",          [&](auto& a) { TimeManager::sleepShell(a); }},
        {"date",           [&](auto&)   { TimeManager::printDateTime(); }},
        {"prime",          [&](auto& a) { PrimeCounter::countPrimes(a); }},
        {"sysinfo",        [&](auto&)   { SystemInfoManager::showSystemInfo(); }},
        {"history",        [&](auto&)   { HistoryManager::showHistory(); }},
        {"taixiu",         [&](auto& a) { playTaiXiu(a); }},

        {"help",           [&](auto& a)   { Builtins::helpCommand(a); }},
        {"exit",           [&](auto& a)   { Builtins::exitCommand(a); }},
    };
}

bool CommandWrapper::executeCommand(const std::vector<std::string>& args, bool isBackground) {
    if (args.empty()) return false;
    auto it = internalCommands.find(args[0]);
    if (it == internalCommands.end()) return false;

    std::vector<std::string> subArgs(args.begin() + 1, args.end());
    if (isBackground) {
        BackgroundManager::executeInternalCommandBackground(subArgs, it->second);
    } else {
        try {
            it->second(subArgs);
        } catch (const std::exception& e) {
            std::cerr << "Command error: " << e.what() << std::endl;
        }
    }
    return true;
}

bool CommandWrapper::isInternalCommand(const std::string& command) {
    return internalCommands.find(command) != internalCommands.end();
}

std::vector<std::string> CommandWrapper::getInternalCommands() {
    std::vector<std::string> commands;
    for (const auto& pair : internalCommands) {
        commands.push_back(pair.first);
    }
    return commands;
}

void CommandWrapper::addCommand(const std::string& name, CommandFunction function) {
    internalCommands[name] = function;
}

void CommandWrapper::removeCommand(const std::string& name) {
    internalCommands.erase(name);
}
