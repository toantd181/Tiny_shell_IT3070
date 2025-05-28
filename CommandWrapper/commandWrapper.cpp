#include "commandWrapper.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <iostream>
#include "../Features/processManager.h"

namespace CommandWrapper {
    // Map từ tên lệnh -> hàm xử lý
    static std::unordered_map<std::string, CommandFunction> commands;

    // Mutex để bảo vệ map khi đăng ký (nếu bạn register từ nhiều thread)
    static std::mutex cmdMutex;

    void initialize() {
        std::lock_guard<std::mutex> lk(cmdMutex);
        commands.clear();
    }

    void registerCommand(const std::string& name, CommandFunction cmd) {
        std::lock_guard<std::mutex> lk(cmdMutex);
        commands[name] = std::move(cmd);
    }

    bool executeCommand(const std::vector<std::string>& args, bool isBackground) {
    if (args.empty()) return false;

    // 1) Lookup
    CommandFunction fn;
    {
        std::lock_guard<std::mutex> lk(cmdMutex);
        auto it = commands.find(args[0]);
        if (it == commands.end()) return false;
        fn = it->second;
    }

    // 2) Strip off the command name
    std::vector<std::string> subArgs;
    if (args.size() > 1)
        subArgs.assign(args.begin() + 1, args.end());

    // 3a) Background
    if (isBackground) {
        auto job = ProcessManager::addJob(args[0]);
        std::thread([fn, subArgs, job]() {
            try {
                fn(subArgs);
            } catch (const std::exception& e) {
                std::cerr << "[Job " << job << "] Error: " << e.what() << "\n";
            }
            ProcessManager::finishJob(job);
            std::cout << "[Job " << job << "] Completed\n";
        }).detach();

        std::cout << "[Job " << job << "] Started\n";
        return true;
    }

    // 3b) Foreground
    try {
        fn(subArgs);
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << "\n";
    }
    return true;
}



    std::vector<std::string> listCommands() {
        std::lock_guard<std::mutex> lk(cmdMutex);
        std::vector<std::string> names;
        names.reserve(commands.size());
        for (auto& kv : commands) {
            names.push_back(kv.first);
        }
        return names;
    }
}
