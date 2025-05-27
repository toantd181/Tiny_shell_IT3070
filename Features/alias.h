#ifndef ALIAS_H
#define ALIAS_H

#include <iostream>
#include <string>
#include <unordered_map>

class AliasManager {
public:
    AliasManager() {
        validCommands = {
            {"copy", "copy"},
            {"make_dir", "make_dir"},
            {"delete_dir", "delete_dir"}
        };
    }

    void addAlias(const std::string& aliasName, const std::string& actualCommand) {
        if (!isValidCommand(actualCommand)) {
            std::cerr << "Error: Command '" << actualCommand << "' is not supported.\n";
            return;
        }

        if (isValidCommand(aliasName)) {
            std::cerr << "Error: Alias name '" << aliasName << "' conflicts with an existing command.\n";
            return;
        }

        if (createsAliasLoop(aliasName, actualCommand)) {
            std::cerr << "Error: Alias '" << aliasName << "' creates a loop!\n";
            return;
        }

        aliases[aliasName] = actualCommand;
        std::cout << "Alias added: " << aliasName << " -> " << actualCommand << "\n";
    }

    void removeAlias(const std::string& name) {
        if (aliases.erase(name)) {
            std::cout << "Alias removed: " << name << "\n";
        } else {
            std::cerr << "Alias not found: " << name << "\n";
        }
    }

    void listAliases() const {
        if (aliases.empty()) {
            std::cout << "No aliases defined.\n";
            return;
        }
        for (const auto& [alias, command] : aliases) {
            std::cout << alias << " -> " << command << "\n";
        }
    }

    std::string resolveAlias(const std::string& input) const {
        std::string resolved = input;
        int hop = 0;
        while (aliases.find(resolved) != aliases.end()) {
            resolved = aliases.at(resolved);
            if (++hop > 10) {
                std::cerr << "Error: Alias resolution exceeded limit for '" << input << "'\n";
                return input;
            }
        }
        return resolved;
    }

    void addValidCommand(const std::string& command) {
        validCommands[command] = command;
    }

private:
    std::unordered_map<std::string, std::string> aliases;
    std::unordered_map<std::string, std::string> validCommands;

    bool isValidCommand(const std::string& cmd) const {
        return validCommands.find(cmd) != validCommands.end();
    }

    bool createsAliasLoop(const std::string& alias, const std::string& command) const {
        std::string next = command;
        int depth = 0;

        while (aliases.find(next) != aliases.end()) {
            next = aliases.at(next);
            if (next == alias || ++depth > 10) return true;
        }

        return false;
    }
};

#endif // ALIAS_H
