// Features/features.cpp
#include "scriptRunner.h"
#include "features.h"
#include "../CommandWrapper/commandWrapper.h"
#include "file.h"
#include "directory.h"
#include "systemInfoManager.h"
#include "historyManager.h"
#include "timeManager.h"
#include "../Processes/primeCounter.h"
#include "../Processes/taixiu.h"
#include "../Features/processManager.h"
#include "shellPath.h"

#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace CommandWrapper;

//--------------------------------------------------------------------------------
// run_s: assemble, link and exec a .s file (foreground or background)
//--------------------------------------------------------------------------------
static void run_s(const std::vector<std::string>& args, bool isBackground) {
    if (args.empty()) {
        std::cerr << "Usage: run_s <source.s>\n";
        return;
    }

    // generate per-process temp names
    std::string id  = std::to_string(getpid()) + "_" + std::to_string(rand());
    std::string obj = "tmp_" + id + ".o";
    std::string bin = "tmp_" + id + ".bin";

    // Assemble
    if (system(("as " + args[0] + " -o " + obj).c_str()) != 0) {
        std::cerr << "Assembly failed\n";
        return;
    }
    // Link (using gcc to respect _start)
    if (system(("gcc -nostartfiles -o " + bin + " " + obj).c_str()) != 0) {
        std::cerr << "Linking failed\n";
        std::remove(obj.c_str());
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    }
    else if (pid == 0) {
        // child
        execl(("./" + bin).c_str(), bin.c_str(), nullptr);
        perror("execl");
        _exit(1);
    }
    else {
        // parent
        if (isBackground) {
            auto job = ProcessManager::addJob("run_s " + args[0]);
            std::cout << "[Job " << job << "] Started run_s " << args[0] << "\n";
            std::thread([pid, bin, obj, job]() {
                int status;
                waitpid(pid, &status, 0);
                ProcessManager::finishJob(job);
                std::cout << "[Job " << job << "] Completed run_s, exit="
                          << WEXITSTATUS(status) << "\n";
                std::remove(obj.c_str());
                std::remove(bin.c_str());
            }).detach();
        } else {
            int status;
            waitpid(pid, &status, 0);
            std::cout << "[run_s] Completed, exit=" << WEXITSTATUS(status) << "\n";
            std::remove(obj.c_str());
            std::remove(bin.c_str());
        }
    }
}

static void run_s_fg(const std::vector<std::string>& args) {
    run_s(args, /*isBackground=*/false);
}

//--------------------------------------------------------------------------------
// registerAllFeatures: wire up every built-in command
//--------------------------------------------------------------------------------
void registerAllFeatures() {
    static FileManager      fm;
    static DirectoryManager dm;

    // Initialize your own PATH to just the current directory
    ShellPath::initialize({ "." });

    // File commands
    registerCommand("create_file", [&](auto& a){ fm.createFile(a); });
    registerCommand("delete_file", [&](auto& a){ fm.deleteFile(a); });
    registerCommand("copy_file",   [&](auto& a){ fm.copyFile(a);   });
    registerCommand("move_file",   [&](auto& a){ fm.moveFile(a);   });
    registerCommand("list_files",  [&](auto& a){ fm.listFiles(a);  });

    // Directory commands
    registerCommand("create_dir",    [&](auto& a){ dm.createDirectory(a);    });
    registerCommand("delete_dir",    [&](auto& a){ dm.deleteDirectory(a);    });
    registerCommand("list_dir_tree", [&](auto& a){ dm.listDirectoryTree(a); });
    registerCommand("move_dir",      [&](auto& a){ dm.moveDirectory(a);     });
    registerCommand("change_dir",    [&](auto& a){ dm.changeDirectory(a);   });

    // Prime & TaiXiu
    registerCommand("prime",   [&](auto& a){ PrimeCounter::countPrimes(a); });
    registerCommand("taixiu",  [&](auto& a){ playTaiXiu(a);               });

    // System info
    registerCommand("sysinfo", [&](auto&){ SystemInfoManager::showSystemInfo(); });

    // History & Time
    registerCommand("history", [&](auto&){ HistoryManager::showHistory(); });
    registerCommand("clear_history", [&](auto&){ HistoryManager::clearHistory(); });
    registerCommand("sliip",   [&](auto& a){ TimeManager::sleepShell(a);    });
    registerCommand("date",    [&](auto&){ TimeManager::printDateTime();  });

    // Screen
    registerCommand("write_to_screen", [&](auto& a){ ScreenManager::writeToScreen(a); });
    registerCommand("read_file",       [&](auto& a){ ScreenManager::readFile(a);       });
    registerCommand("clear_screen",    [&](auto&){    ScreenManager::clearTerminal(); });

    // Script runner
    registerCommand("run_sh", [&](auto& a){ ScriptRunner::run_sh(a); });

    // **ShellPath**-based PATH management
    registerCommand("path", [&](auto&){
        for (auto& d : ShellPath::get()) std::cout << d << "\n";
    });
    registerCommand("addpath", [&](auto& a){
        if (a.empty()) {
            std::cerr << "Usage: addpath <dir> [front|back]\n";
            return;
        }
        bool front = (a.size()>1 && a[1]=="front");
        ShellPath::add(a[0], front);
    });

    // Process control
    registerCommand("ps",     [&](auto&){ ProcessManager::listProcesses({}); });
    registerCommand("kill",   [&](auto& a){ ProcessManager::killProcess(a);   });
    registerCommand("stop",   [&](auto& a){ ProcessManager::stopProcess(a);   });
    registerCommand("resume", [&](auto& a){ ProcessManager::resumeProcess(a); });

    // Assembly runner
    registerCommand("run_s",    run_s_fg);
    registerCommand("run_s&", [&](auto& a){ run_s(a, /*isBackground=*/true); });
}
