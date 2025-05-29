// shellPath.cpp
#include "shellPath.h"
#include <filesystem>

namespace ShellPath {
    static std::vector<std::string> tinyPath;

    void initialize(const std::vector<std::string>& defaults) {
        tinyPath = defaults;
    }

    const std::vector<std::string>& get() {
        return tinyPath;
    }

    void add(const std::string& dir, bool front) {
        if (front) tinyPath.insert(tinyPath.begin(), dir);
        else       tinyPath.push_back(dir);
    }

    std::string findExecutable(const std::string& name) {
        namespace fs = std::filesystem;
        for (auto& d : tinyPath) {
            fs::path p = fs::path(d) / name;
            if (fs::exists(p) && fs::is_regular_file(p) 
                               && (fs::status(p).permissions() & fs::perms::owner_exec) != fs::perms::none) {
                return p.string();
            }
        }
        return "";
    }
}
