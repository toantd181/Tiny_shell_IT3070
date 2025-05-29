// shellPath.h
#pragma once
#include <vector>
#include <string>

namespace ShellPath {
    // Initialize at startup
    void initialize(const std::vector<std::string>& defaults);

    // Get a copy of the current list
    const std::vector<std::string>& get();

    // Add a new entry (front or back)
    void add(const std::string& dir, bool front = false);

    // Find an executable in these directories
    // Returns full path or empty
    std::string findExecutable(const std::string& name);
}
