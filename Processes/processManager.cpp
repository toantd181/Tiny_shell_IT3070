#include "processManager.h"
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

namespace ProcessManager {
    void listProcesses() {
        std::cout << "Process listing not implemented yet\n";
    }

    void killProcess(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "Usage: kill <pid>\n";
            return;
        }
        try {
            pid_t pid = std::stoi(args[0]);
            if (kill(pid, SIGTERM) != 0) {
                perror("kill failed");
            }
        } catch (const std::exception& e) {
            std::cerr << "Invalid PID: " << e.what() << "\n";
        }
    }

    void stopProcess(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "Usage: stop <pid>\n";
            return;
        }
        try {
            pid_t pid = std::stoi(args[0]);
            if (kill(pid, SIGSTOP) != 0) {
                perror("stop failed");
            }
        } catch (const std::exception& e) {
            std::cerr << "Invalid PID: " << e.what() << "\n";
        }
    }

    void resumeProcess(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "Usage: resume <pid>\n";
            return;
        }
        try {
            pid_t pid = std::stoi(args[0]);
            if (kill(pid, SIGCONT) != 0) {
                perror("resume failed");
            }
        } catch (const std::exception& e) {
            std::cerr << "Invalid PID: " << e.what() << "\n";
        }
    }
} 