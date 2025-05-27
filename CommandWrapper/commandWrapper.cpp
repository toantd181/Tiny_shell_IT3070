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
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <atomic>

using namespace std;

enum ProcessStatus { RUNNING, STOPPED };

struct ProcessInfo {
    pid_t pid;
    std::string command;
    ProcessStatus status;
};

std::vector<ProcessInfo> processList;

// Manager instances
static DirectoryManager directoryManager;
static FileManager fileManager;
static ScreenManager screenManager;
static AliasManager aliasManager;



namespace ProcessManager {

void listProcesses() {
    std::cout << "PID\tStatus\t\tCommand\n";
    for (const auto& p : processList) {
        std::cout << p.pid << "\t" 
                  << (p.status == RUNNING ? "Running" : "Stopped") << "\t\t" 
                  << p.command << "\n";
    }
}

void killProcess(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: kill <pid>\n";
        return;
    }
    int pid = std::stoi(args[0]);
    
    // First check if it's one of our background jobs
    for (auto it = processList.begin(); it != processList.end(); ++it) {
        if (it->pid == pid) {
            processList.erase(it);
            std::cout << "Background job " << pid << " terminated\n";
            return;
        }
    }
    
    // If not found in our list, try to kill as a real process
    if (kill(pid, SIGKILL) == -1) {
        perror("kill");
    }
}

void stopProcess(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: stop <pid>\n";
        return;
    }
    int pid = std::stoi(args[0]);
    
    // First check if it's one of our background jobs
    for (auto& p : processList) {
        if (p.pid == pid) {
            p.status = STOPPED;
            std::cout << "Background job " << pid << " stopped\n";
            return;
        }
    }
    
    // If not found in our list, try to stop as a real process
    if (kill(pid, SIGSTOP) == -1) {
        perror("stop");
    }
}

void resumeProcess(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: resume <pid>\n";
        return;
    }
    int pid = std::stoi(args[0]);
    
    // First check if it's one of our background jobs
    for (auto& p : processList) {
        if (p.pid == pid) {
            p.status = RUNNING;
            std::cout << "Background job " << pid << " resumed\n";
            return;
        }
    }
    
    // If not found in our list, try to resume as a real process
    if (kill(pid, SIGCONT) == -1) {
        perror("resume");
    }
}
}


namespace EnvironmentManager {

void showPath() {
    const char* path = std::getenv("PATH");
    if (path) std::cout << "PATH: " << path << "\n";
    else std::cerr << "PATH not set\n";
}

void addPath(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: addpath <new_path>\n";
        return;
    }
    const char* oldPath = std::getenv("PATH");
    std::string newPath = args[0];
    std::string updated = oldPath ? std::string(oldPath) + ":" + newPath : newPath;
    setenv("PATH", updated.c_str(), 1);
    std::cout << "PATH updated: " << updated << "\n";
}
}

// Background Process Manager (local to this file)
class BackgroundManager {
private:
    static std::vector<std::thread> backgroundThreads;
    static int nextJobId;
    static std::atomic<pid_t> nextPid;

public:
    static void executeInternalCommandBackground(const std::vector<std::string>& args, CommandFunction command, const std::string& cmdName) {
        int jobId = ++nextJobId;
        pid_t pid = 10000 + (++nextPid);  // Start PIDs from 10000
        std::cout << "[Background Job " << jobId << "] Started" << std::endl;

        // Add to process list with a proper PID
        processList.push_back({pid, cmdName, RUNNING});

        backgroundThreads.emplace_back([args, command, jobId, pid]() {
            try {
                command(args);
                std::cout << "[Background Job " << jobId << "] Completed" << std::endl;
                // Remove from process list when done
                for (auto it = processList.begin(); it != processList.end(); ++it) {
                    if (it->pid == pid) {
                        processList.erase(it);
                        break;
                    }
                }
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



void addProcess(pid_t pid, const std::string& command) {
    processList.push_back({pid, command, RUNNING});
}



void stopProcess(pid_t pid) {
    if (kill(pid, SIGSTOP) == 0) {
        for (auto& p : processList) {
            if (p.pid == pid) {
                p.status = STOPPED;
                break;
            }
        }
    } else {
        perror("stop failed");
    }
}

void resumeProcess(pid_t pid) {
    if (kill(pid, SIGCONT) == 0) {
        for (auto& p : processList) {
            if (p.pid == pid) {
                p.status = RUNNING;
                break;
            }
        }
    } else {
        perror("resume failed");
    }
}

void killProcess(pid_t pid) {
    if (kill(pid, SIGKILL) == 0) {
        for (auto it = processList.begin(); it != processList.end(); ++it) {
            if (it->pid == pid) {
                processList.erase(it);
                break;
            }
        }
    } else {
        perror("kill failed");
    }
}

std::vector<std::thread> BackgroundManager::backgroundThreads;
int BackgroundManager::nextJobId = 0;
std::atomic<pid_t> BackgroundManager::nextPid = 0;

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
        {"history",        [&](auto& a) { HistoryManager::showHistory(a); }},
        {"clear_history",  [&](auto& a) { HistoryManager::clearHistory(); }},
        {"taixiu",         [&](auto& a) { playTaiXiu(a); }},

        {"help",           [&](auto& a)   { Builtins::helpCommand(a); }},
        {"exit",           [&](auto& a)   { Builtins::exitCommand(a); }},

        // Process
        {"ps",        [&](auto&)   { ProcessManager::listProcesses(); }},
        {"kill",      [&](auto& a) { ProcessManager::killProcess(a); }},
        {"stop",      [&](auto& a) { ProcessManager::stopProcess(a); }},
        {"resume",    [&](auto& a) { ProcessManager::resumeProcess(a); }},

        // Environment
        {"path",      [&](auto&)   { EnvironmentManager::showPath(); }},
        {"addpath",   [&](auto& a) { EnvironmentManager::addPath(a); }},

    };
}

bool CommandWrapper::executeCommand(const std::vector<std::string>& args, bool isBackground) {
    if (args.empty()) return false;

    auto it = internalCommands.find(args[0]);
    std::vector<std::string> subArgs(args.begin() + 1, args.end());

    if (it != internalCommands.end()) {
        if (isBackground) {
            // Pass the command name for display
            BackgroundManager::executeInternalCommandBackground(subArgs, it->second, args[0]);
        } else {
            try {
                it->second(subArgs);
            } catch (const std::exception& e) {
                std::cerr << "Command error: " << e.what() << std::endl;
            }
        }
        return true;
    }

    // Nếu không phải internal command thì fork & exec
    pid_t pid = fork();
    if (pid == 0) {
        std::vector<char*> cargs;
        for (const auto& arg : args) cargs.push_back(const_cast<char*>(arg.c_str()));
        cargs.push_back(nullptr);
        execvp(cargs[0], cargs.data());
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        if (!isBackground) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            std::cout << "[Background PID " << pid << "] started\n";
        }
        addProcess(pid, args[0]);  // Ghi nhận lại tiến trình
    } else {
        perror("fork failed");
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
