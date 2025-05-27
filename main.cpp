// main.cpp (Refactored)
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <csignal>
#include "CommandWrapper/commandWrapper.h"

// Tokenize input string into arguments
std::vector<std::string> tokenize(const std::string& input) {
    std::istringstream stream(input);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Placeholder: execute external command (you can implement later)
void executeExternalCommand(const std::vector<std::string>& args, bool isBackground) {
    std::cerr << "[External Command] Not implemented: ";
    for (const auto& arg : args) std::cerr << arg << ' ';
    std::cerr << std::endl;
}

// Optional: Handle Ctrl+C
void handleSigInt(int sig) {
    std::cout << "\n[Signal] Interrupt received (Ctrl+C)\n";
    // Optionally restore prompt or clean up
}

int main() {
    std::signal(SIGINT, handleSigInt); // Handle Ctrl+C
    CommandWrapper::initializeCommands();

    std::string input;
    while (true) {
        std::cout << "tiny_shell> ";
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::vector<std::string> args = tokenize(input);
        if (args.empty()) continue;

        bool isBackground = false;
        if (!args.empty() && args.back() == "&") {
            isBackground = true;
            args.pop_back();
        }

        // Try internal command first
        if (CommandWrapper::executeCommand(args, isBackground)) {
            continue;
        }

        // Fallback to external command
        executeExternalCommand(args, isBackground);
    }

    return 0;
}
