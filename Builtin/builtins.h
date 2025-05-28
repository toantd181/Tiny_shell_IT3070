// builtins.h
#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

class Builtins {
public:
    using BuiltinFunction = std::function<void(const std::vector<std::string>&)>;

    static void initialize(); // Đăng ký các lệnh nội bộ
    static bool isBuiltin(const std::string& cmd);
    static bool execute(const std::vector<std::string>& args);

    // Built-in commands
    static void helpCommand(const std::vector<std::string>& args);
    static void exitCommand(const std::vector<std::string>& args);
    static void dateCommand(const std::vector<std::string>& args);  // optional

private:
    static std::unordered_map<std::string, BuiltinFunction> builtinCommands;
};

#endif
