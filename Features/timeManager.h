#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <chrono>
#include <thread>
#include <iostream>
#include <ctime>
#include <vector>
#include "processManager.h"

class TimeManager {
public:
    static void sleepShell(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "Usage: sliip <seconds>\n";
            return;
        }

        try {
            int seconds = std::stoi(args[0]);
            std::this_thread::sleep_for(std::chrono::seconds(seconds));
        } catch (...) {
            std::cerr << "Invalid time format\n";
        }
    }

    static void printDateTime(const std::vector<std::string>& args = {}) {
        std::time_t now = std::time(nullptr);
        std::cout << "Current date and time: " << std::ctime(&now);
    }
};

#endif
