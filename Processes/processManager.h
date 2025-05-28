#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <vector>
#include <string>
#include <mutex>

namespace ProcessManager {
    void listProcesses();
    void killProcess(const std::vector<std::string>& args);
    void stopProcess(const std::vector<std::string>& args);
    void resumeProcess(const std::vector<std::string>& args);
}

#endif // PROCESS_MANAGER_H 