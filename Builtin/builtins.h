#ifndef BUILTINS_H
#define BUILTINS_H

#include <vector>
#include <string>

namespace Builtins {
    void helpCommand(const std::vector<std::string>& args);
    void exitCommand(const std::vector<std::string>& args);
}

#endif
