#ifndef ALIAS_H
#define ALIAS_H

#include <iostream>
#include <string>
#include <unordered_map>

class AliasManager {
public:
    AliasManager() {
        // Khởi tạo danh sách lệnh hợp lệ
        validCommands = {
            {"copy", "copy"},
            {"make_dir", "make_dir"},
            {"delete_dir", "delete_dir"}
        };
    }

    void addAlias(const std::string &name, const std::string &command) {
        if (validCommands.find(command) == validCommands.end()) {
            std::cerr << "Error: Command '" << command << "' is not supported." << std::endl;
            return;
        }

        if (validCommands.find(name) != validCommands.end()) {
            std::cerr << "Error: Alias name '" << name << "' conflicts with an existing command." << std::endl;
            return;
        }

        // Ngăn chặn alias vòng lặp
        if (checkAliasLoop(name, command)) {
            std::cerr << "Error: Alias '" << name << "' creates a loop!" << std::endl;
            return;
        }

        aliases[name] = command;
        std::cout << "Alias added: " << name << " -> " << command << std::endl;
    }

    void removeAlias(const std::string &name) {
        if (aliases.erase(name)) {
            std::cout << "Alias removed: " << name << std::endl;
        } else {
            std::cerr << "Alias not found: " << name << std::endl;
        }
    }

    void listAliases() const {
        for (const auto &alias : aliases) {
            std::cout << alias.first << " -> " << alias.second << std::endl;
        }
    }

    std::string resolveAlias(const std::string &command) const {
        std::string resolved = command;
        int count = 0;
        
        while (aliases.find(resolved) != aliases.end()) {
            resolved = aliases.at(resolved);
            if (++count > 10) { // Giới hạn để tránh vòng lặp vô hạn
                std::cerr << "Error: Alias resolution exceeded limit for '" << command << "'!" << std::endl;
                return command;
            }
        }

        return resolved;
    }

    void addValidCommand(const std::string &command) {
        validCommands[command] = command;
    }

private:
    std::unordered_map<std::string, std::string> aliases;
    std::unordered_map<std::string, std::string> validCommands;

    bool checkAliasLoop(const std::string &alias, const std::string &command) {
        std::string check = command;
        while (aliases.find(check) != aliases.end()) {
            check = aliases[check];
            if (check == alias) return true;
        }
        return false;
    }
};

#endif // ALIAS_H
