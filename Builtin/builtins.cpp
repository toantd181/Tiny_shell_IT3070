// builtins.cpp
#include "builtins.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

std::unordered_map<std::string, Builtins::BuiltinFunction> Builtins::builtinCommands;

void Builtins::initialize() {
    builtinCommands["help"] = helpCommand;
    builtinCommands["exit"] = exitCommand;
    builtinCommands["quit"] = exitCommand; // alias
    
}

bool Builtins::isBuiltin(const std::string& cmd) {
    return builtinCommands.find(cmd) != builtinCommands.end();
}

bool Builtins::execute(const std::vector<std::string>& args) {
    if (args.empty()) return false;
    auto it = builtinCommands.find(args[0]);
    if (it != builtinCommands.end()) {
        it->second(args);  // Gọi lệnh nội bộ
        return true;
    }
    return false;
}

void Builtins::helpCommand(const std::vector<std::string>&) {
    std::cout << "Available built-in commands:\n";
    for (const auto& cmd : builtinCommands) {
        std::cout << "  " << cmd.first << "\n";
    }
    std::cout << "\nUse '&' at the end of a command to run it in the background.\n";
}

void Builtins::exitCommand(const std::vector<std::string>&) {
    std::cout << "Shell exited. Goodbye!\n";
    std::exit(0);
}
