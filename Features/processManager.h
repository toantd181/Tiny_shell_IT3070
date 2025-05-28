// processManager.h
#pragma once

#include <vector>
#include <string>
#include <sys/types.h>   // for pid_t
#include <atomic>

namespace ProcessManager {

    using JobId = int;

    // Called when you spawn a background built-in thread
    JobId addJob(const std::string& cmdName);

    // Called from child-forked processes
    void addProcess(pid_t pid, const std::string& commandLine);

    void listProcesses(const std::vector<std::string>& args);
    void killProcess  (const std::vector<std::string>& args);
    void stopProcess  (const std::vector<std::string>& args);
    void resumeProcess(const std::vector<std::string>& args);

    // Helpers for background-job bookkeeping
    void finishJob(JobId id);

    // **Extern declaration** of your atomic counter
    extern std::atomic<JobId> nextJobId;
}
