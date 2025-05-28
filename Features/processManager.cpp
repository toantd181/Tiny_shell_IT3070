// processManager.cpp
#include "processManager.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

namespace ProcessManager {

    enum class Status { Running, Stopped };

    struct ProcessInfo {
        pid_t pid;
        std::string cmd;
        Status status;
    };

    // These remain static inside the .cpp
    static std::vector<ProcessInfo> processList;
    static std::mutex              processMutex;

    // **Single definition** of the atomic counter**
    std::atomic<JobId> nextJobId{1};

    JobId addJob(const std::string& cmdName) {
        std::lock_guard<std::mutex> lk(processMutex);
        JobId id = nextJobId++;
        processList.push_back({ id, cmdName, Status::Running });
        return id;
    }

    void finishJob(JobId id) {
        std::lock_guard<std::mutex> lk(processMutex);
        for (auto& p : processList) {
            if (p.pid == id) {
                p.status = Status::Stopped;
                return;
            }
        }
    }

    void addProcess(pid_t pid, const std::string& commandLine) {
        std::lock_guard<std::mutex> lk(processMutex);
        processList.push_back({ pid, commandLine, Status::Running });
    }

    void listProcesses(const std::vector<std::string>&) {
        std::lock_guard<std::mutex> lk(processMutex);
        std::cout << "PID/JobID\tStatus\t\tCommand\n";
        for (auto& p : processList) {
            std::cout
                << p.pid << "\t"
                << (p.status == Status::Running ? "Running" : "Stopped")
                << "\t\t" << p.cmd << "\n";
        }
    }

    static void signalAndUpdate(const std::vector<std::string>& args,
                                int sig, Status newStatus, bool removeOnKill)
    {
        if (args.empty()) {
            std::cerr << "Usage: kill|stop|resume <pid|jobid>\n";
            return;
        }
        int id = std::stoi(args[0]);
        
        std::lock_guard<std::mutex> lk(processMutex);
        for (auto it = processList.begin(); it != processList.end(); ++it) {
            if (it->pid == id) {
                // For job IDs (our internal processes), just update status
                if (id < 10000) {  // Job IDs are small numbers
                    if (removeOnKill) {
                        processList.erase(it);
                    } else {
                        it->status = newStatus;
                    }
                    return;
                }
                // For system PIDs, try to send signal
                if (::kill(id, sig) < 0) {
                    perror("kill");
                    return;
                }
                if (removeOnKill) {
                    processList.erase(it);
                } else {
                    it->status = newStatus;
                }
                return;
            }
        }
        std::cerr << "No such process or job: " << id << "\n";
    }

    void killProcess(const std::vector<std::string>& args) {
        signalAndUpdate(args, SIGKILL, Status::Running, /*removeOnKill=*/true);
    }

    void stopProcess(const std::vector<std::string>& args) {
        signalAndUpdate(args, SIGSTOP, Status::Stopped, /*removeOnKill=*/false);
    }

    void resumeProcess(const std::vector<std::string>& args) {
        signalAndUpdate(args, SIGCONT, Status::Running, /*removeOnKill=*/false);
    }

} // namespace ProcessManager
