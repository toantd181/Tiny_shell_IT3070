#include "builtins.h"
#include "../CommandWrapper/commandWrapper.h" // Nếu cần dùng CommandWrapper::getInternalCommands()
#include <iostream>
#include <cstdlib>

using namespace std;

void Builtins::helpCommand(const vector<string>& args) {
    cout << "Available internal commands:" << endl;
    auto commands = CommandWrapper::getInternalCommands();
    for (const auto& cmd : commands) {
        cout << "  " << cmd << endl;
    }
    cout << "\nUse '&' at the end of any command to run it in background" << endl;
    cout << "Example: sliip 10 &" << endl;
}

void Builtins::exitCommand(const vector<string>& args) {
    cout << "Shell exited. Goodbye!" << endl;
    exit(0); // Gọi exit trực tiếp thay vì return về main
}
