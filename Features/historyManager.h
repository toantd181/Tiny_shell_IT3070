#ifndef HISTORY_MANAGER_H
#define HISTORY_MANAGER_H

#include <vector>
#include <string>
#include <iostream>

class HistoryManager {
private:
    static inline std::vector<std::string> history;

public:
    static void addCommand(const std::string &command) {
        history.push_back(command);
    }

    static void showHistory(const std::vector<std::string>& args = {}) {
        std::cout << "Command History:\n";
        for (size_t i = 0; i < history.size(); ++i) {
            std::cout << i + 1 << ": " << history[i] << "\n";
        }
    }
};

#endif
