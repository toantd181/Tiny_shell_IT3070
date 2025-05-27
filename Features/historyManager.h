#ifndef HISTORY_MANAGER_H
#define HISTORY_MANAGER_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

class HistoryManager {
private:
    static inline std::vector<std::string> history;
    static inline size_t maxHistorySize = 1000; // Limit history size

public:
    static void addCommand(const std::string &command) {
        // Don't add empty commands or duplicate consecutive commands
        if (command.empty() || (!history.empty() && history.back() == command)) {
            return;
        }
        
        history.push_back(command);
        
        // Keep history size manageable
        if (history.size() > maxHistorySize) {
            history.erase(history.begin());
        }
    }

    static void showHistory(const std::vector<std::string>& args = {}) {
        if (history.empty()) {
            std::cout << "No command history available.\n";
            return;
        }

        size_t start = 0;
        size_t count = history.size();
        
        // Handle optional arguments for showing last N commands
        if (!args.empty()) {
            try {
                int n = std::stoi(args[0]);
                if (n > 0) {
                    count = static_cast<size_t>(n);
                    if (count > history.size()) {
                        count = history.size();
                    }
                    start = history.size() - count;
                }
            } catch (const std::exception&) {
                std::cerr << "Invalid argument for history command. Usage: history [n]\n";
                return;
            }
        }

        std::cout << "Command History:\n";
        std::cout << std::string(40, '-') << "\n";
        
        for (size_t i = start; i < start + count; ++i) {
            std::cout << std::setw(4) << (i + 1) << ": " << history[i] << "\n";
        }
        
        std::cout << std::string(40, '-') << "\n";
        std::cout << "Total commands: " << history.size() << "\n";
    }
    
    static void clearHistory() {
        history.clear();
        std::cout << "Command history cleared.\n";
    }
    
    static const std::vector<std::string>& getHistory() {
        return history;
    }
    
    static std::string getCommand(size_t index) {
        if (index > 0 && index <= history.size()) {
            return history[index - 1];
        }
        return "";
    }
    
    static size_t getHistorySize() {
        return history.size();
    }
};

#endif