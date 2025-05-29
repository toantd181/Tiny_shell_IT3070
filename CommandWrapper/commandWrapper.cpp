#include "commandWrapper.h"
#include "../Features/shellPath.h"
#include "../Features/processManager.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstring>

namespace CommandWrapper {

static std::unordered_map<std::string, CommandFunction> commandMap;
static std::mutex cmdMutex;

void initialize() {
    std::lock_guard<std::mutex> lk(cmdMutex);
    commandMap.clear();
}

void registerCommand(const std::string& name, CommandFunction fn) {
    std::lock_guard<std::mutex> lk(cmdMutex);
    commandMap[name] = std::move(fn);
}

bool executeCommand(const std::vector<std::string>& args, bool isBackground) {
    if (args.empty()) return true;
    const auto cmd = args[0];

    // 1) Built-in?
    CommandFunction fn;
    {
        std::lock_guard<std::mutex> lk(cmdMutex);
        auto it = commandMap.find(cmd);
        if (it != commandMap.end()) fn = it->second;
    }
    if (fn) {
        // strip off command name
        std::vector<std::string> sub(args.begin()+1, args.end());
        // background built-in?
        if (isBackground) {
            auto job = ProcessManager::addJob(cmd);
            std::cout << "[Job " << job << "] Started " << cmd << "\n";
            std::thread([fn, sub, job](){
                fn(sub);
                ProcessManager::finishJob(job);
                std::cout << "[Job " << job << "] Completed\n";
            }).detach();
        } else {
            fn(std::vector<std::string>(args.begin()+1, args.end()));
        }
        return true;
    }

    // 2) External: find path or literal slash
    std::string full;
    if (cmd.find('/') != std::string::npos) {
        full = cmd;
    } else {
        full = ShellPath::findExecutable(cmd);
    }
    if (full.empty()) {
        std::cerr << "Unknown command: " << cmd << "\n";
        return false;
    }

    // build argv
    std::vector<char*> argv;
    argv.reserve(args.size()+1);
    argv.push_back(const_cast<char*>(full.c_str()));
    for (size_t i = 1; i < args.size(); ++i)
        argv.push_back(const_cast<char*>(args[i].c_str()));
    argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return false;
    }
    if (pid == 0) {
        execv(full.c_str(), argv.data());
        perror("execv");
        _exit(1);
    } else {
        if (isBackground) {
            std::cout << "[Background PID " << pid << "] started\n";
            ProcessManager::addProcess(pid, cmd);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
    return true;
}

std::vector<std::string> listCommands() {
    std::lock_guard<std::mutex> lk(cmdMutex);
    std::vector<std::string> names;
    for (auto& kv : commandMap) names.push_back(kv.first);
    return names;
}

}  // namespace CommandWrapper
