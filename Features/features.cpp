// Features/features.cpp
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

#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace CommandWrapper;

//--------------------------------------------------------------------------------
// run_s: assemble, link and exec a .s file
//--------------------------------------------------------------------------------
static void run_s(const std::vector<std::string>& args, bool isBackground = false) {
    if (args.empty()) {
        std::cerr << "Usage: run_s <source.s>\n";
        return;
    }

    std::string src = args[0];
    std::string obj = "tmp_" + std::to_string(getpid()) + ".o";
    std::string bin = "tmp_" + std::to_string(getpid()) + ".bin";

    // Assemble
    if (system(("as " + src + " -o " + obj).c_str()) != 0) {
        std::cerr << "Assembly failed\n";
        return;
    }

    // Link
    if (system(("gcc -nostartfiles -o " + bin + " " + obj).c_str()) != 0) {
        std::cerr << "Linking failed\n";
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl(("./" + bin).c_str(), bin.c_str(), nullptr);
        perror("execl");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        if (isBackground) {
            ProcessManager::addProcess(pid, "run_s " + src);
            std::cout << "[run_s Background] PID " << pid << " started\n";
        } else {
            int status;
            waitpid(pid, &status, 0);
            std::cout << "[run_s] Process finished with status " << WEXITSTATUS(status) << "\n";
            std::cout.flush();
        }

        // Clean up temporary files
        std::remove(obj.c_str());
        std::remove(bin.c_str());
    } else {
        perror("fork");
    }
}

static void run_s_wrapper(const std::vector<std::string>& args) {
    run_s(args, false);
}

//--------------------------------------------------------------------------------
// registerAllFeatures: wire up every built-in command
//--------------------------------------------------------------------------------
void registerAllFeatures() {
    static FileManager        fm;
    static DirectoryManager   dm;

    // File commands
    registerCommand("create_file",  [&](auto& subArgs){ fm.createFile(subArgs);  });
    registerCommand("delete_file",  [&](auto& subArgs){ fm.deleteFile(subArgs);  });
    registerCommand("copy_file",    [&](auto& subArgs){ fm.copyFile(subArgs);    });
    registerCommand("move_file",    [&](auto& subArgs){ fm.moveFile(subArgs);    });
    registerCommand("list_files",   [&](auto& subArgs){ fm.listFiles(subArgs);   });

    // Directory commands
    registerCommand("create_dir",      [&](auto& subArgs){ dm.createDirectory(subArgs);     });
    registerCommand("delete_dir",      [&](auto& subArgs){ dm.deleteDirectory(subArgs);     });
    registerCommand("list_dir_tree",   [&](auto& subArgs){ dm.listDirectoryTree(subArgs);   });
    registerCommand("move_dir",        [&](auto& subArgs){ dm.moveDirectory(subArgs);      });
    registerCommand("change_dir",      [&](auto& subArgs){ dm.changeDirectory(subArgs);    });

    // PrimeCounter
    registerCommand("prime",           [&](auto& subArgs){ PrimeCounter::countPrimes(subArgs); });

    // TaiXiu game
    registerCommand("taixiu",          [&](auto& subArgs){ playTaiXiu(subArgs); });

    // System info
    registerCommand("sysinfo",         [&](auto&){ SystemInfoManager::showSystemInfo(); });

    // History & Time
    registerCommand("history",         [&](auto&){ HistoryManager::showHistory(); });
    registerCommand("sliip",           [&](auto& subArgs){ TimeManager::sleepShell(subArgs); });
    registerCommand("date",            [&](auto&){ TimeManager::printDateTime(); });

    // ScreenManager
    registerCommand("write_to_screen", [&](auto& subArgs){ ScreenManager::writeToScreen(subArgs); });
    registerCommand("read_file",       [&](auto& subArgs){ ScreenManager::readFile(subArgs); });
    registerCommand("clear_screen",    [&](auto&){ ScreenManager::clearTerminal(); });

    // Environment
    registerCommand("path",            [&](auto&){ 
        const char* p = std::getenv("PATH");
        std::cout << (p ? p : "") << "\n";
    });
    registerCommand("addpath",         [&](auto& subArgs){
        if (subArgs.empty()) { std::cerr<<"Usage: addpath <dir>\n"; return; }
        const char* old = std::getenv("PATH");
        std::string v = old ? std::string(old) + ":" + subArgs[0] : subArgs[0];
        setenv("PATH", v.c_str(), 1);
    });

    // Process control
    registerCommand("ps",              [&](auto&){ ProcessManager::listProcesses({}); });
    registerCommand("kill",            [&](auto& subArgs){ ProcessManager::killProcess(subArgs); });
    registerCommand("stop",            [&](auto& subArgs){ ProcessManager::stopProcess(subArgs); });
    registerCommand("resume",          [&](auto& subArgs){ ProcessManager::resumeProcess(subArgs); });

    // Assembly runner
    registerCommand("run_s", run_s_wrapper);
}
