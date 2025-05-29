// main.cpp

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <csignal>
#include <filesystem>

#include "Builtin/builtins.h"
#include "CommandWrapper/commandWrapper.h"
#include "Features/historyManager.h"
#include "Features/features.h"    // registerAllFeatures()
#include "Features/shellPath.h"   // your in-memory PATH

namespace fs = std::filesystem;

// Re-print prompt on Ctrl+C
static void handleSigInt(int) {
    std::cout << "\n\ntiny_shell> ";
    std::cout.flush();
}

// Split input into tokens, supporting single & double quotes
static std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string cur;
    bool inDouble = false, inSingle = false;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (inDouble) {
            if (c == '"')        inDouble = false;
            else if (c == '\\' && i+1 < input.size()) cur += input[++i];
            else cur += c;
        }
        else if (inSingle) {
            if (c == '\'')       inSingle = false;
            else cur += c;
        }
        else {
            if (std::isspace(static_cast<unsigned char>(c))) {
                if (!cur.empty()) {
                    tokens.emplace_back(std::move(cur));
                    cur.clear();
                }
            }
            else if (c == '"')  inDouble = true;
            else if (c == '\'') inSingle = true;
            else cur += c;
        }
    }
    if (!cur.empty()) tokens.emplace_back(std::move(cur));
    return tokens;
}

int main() {
    // 1) Ctrl+C handler
    std::signal(SIGINT, handleSigInt);

    // 2) Initialize shell subsystems
    Builtins::initialize();           // help, exit, date, quit…
    CommandWrapper::initialize();     // clear previous registrations
    registerAllFeatures();            // wire up file/dir/prime/ps/kill/etc
    // (registerAllFeatures also seeds ShellPath::initialize)

    // Remember the directory we started in
    const fs::path root = fs::current_path();

    std::string line;
    while (true) {
        // Compute relative prompt path
        fs::path cwd = fs::current_path();
        fs::path rel = fs::relative(cwd, root);
        std::cout << (rel.empty() ? "." : rel.string()) << " tiny_shell> ";

        if (!std::getline(std::cin, line)) {
            // EOF (Ctrl+D) or error
            std::cout << "\nGoodbye!\n";
            break;
        }
        if (line.empty()) continue;

        // 3) Save to history
        HistoryManager::addCommand(line);

        // 4) Tokenize & detect background (&)
        auto args = tokenize(line);
        bool isBg = false;
        if (!args.empty() && args.back() == "&") {
            isBg = true;
            args.pop_back();
        }
        if (args.empty()) continue;

        // 5) Built-ins (help, exit, etc)
        if (Builtins::execute(args)) continue;

        // 6) Everything else via CommandWrapper
        if (CommandWrapper::executeCommand(args, isBg)) continue;

        // 7) Unknown
        std::cerr << "Unknown command: " << args[0] << "\n";
    }

    return 0;
}
