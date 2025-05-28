#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <csignal>
#include <filesystem>
#include "CommandWrapper/commandWrapper.h"
#include "Features/historyManager.h"
#include "Builtin/builtins.h"
#include "Features/features.h"

// Tokenize input string into arguments
std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string cur;
    bool inDouble = false, inSingle = false;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (inDouble) {
            if (c == '"') {
                inDouble = false;
            } else if (c == '\\' && i + 1 < input.size()) {
                // handle simple escapes inside double quotes
                cur += input[++i];
            } else {
                cur += c;
            }
        }
        else if (inSingle) {
            if (c == '\'') {
                inSingle = false;
            } else {
                cur += c;
            }
        }
        else {
            if (std::isspace(c)) {
                if (!cur.empty()) {
                    tokens.push_back(cur);
                    cur.clear();
                }
            }
            else if (c == '"') {
                inDouble = true;
            }
            else if (c == '\'') {
                inSingle = true;
            }
            else {
                cur += c;
            }
        }
    }
    if (!cur.empty()) {
        tokens.push_back(cur);
    }

    return tokens;
}


void executeExternalCommand(const std::vector<std::string>& args, bool isBackground) {
    std::cerr << "[External Command] Not implemented: ";
    for (const auto& arg : args) std::cerr << arg << ' ';
    std::cerr << std::endl;
}

void handleSigInt(int) {
    std::cout << "\n[Signal] Interrupt received (Ctrl+C)\n";
    std::cout << "tiny_shell> ";
    std::cout.flush();
}

int main() {
    std::signal(SIGINT, handleSigInt);

    Builtins::initialize();         // 1) help, exit, date, quit…
    CommandWrapper::initialize();   // 2) clear any old registrations
    registerAllFeatures();          // 3) file/dir/prime/ps/kill/…, env, run_s…

    // Store the initial working directory as root
    std::filesystem::path root_dir = std::filesystem::current_path();

    std::string input;
    while (true) {
        // Get current working directory and show relative path from root
        std::filesystem::path cwd = std::filesystem::current_path();
        std::filesystem::path relative_path = std::filesystem::relative(cwd, root_dir);
        std::cout << relative_path.string() << " tiny_shell> ";
        if (!std::getline(std::cin, input)) break;  // EOF or error

        if (input.empty()) continue;
        HistoryManager::addCommand(input);

        auto args = tokenize(input);
        bool isBg = (!args.empty() && args.back() == "&");
        if (isBg) args.pop_back();

        if (Builtins::execute(args))                   continue;
        if (CommandWrapper::executeCommand(args, isBg)) continue;

        std::cerr << "Unknown command: " << args[0] << "\n";
    }

    std::cout << "Goodbye!\n";
    return 0;
}