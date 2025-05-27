#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>
#include <thread>
#include <future>

#include "Features/features.h"
#include "Processes/processes.h"
#include "Features/alias.h"

using namespace std;
namespace fs = std::filesystem;

// Khởi tạo các manager
DirectoryManager directoryManager;
FileManager fileManager;
ScreenManager screenManager;
AliasManager aliasManager;

using CommandFunction = function<void(const vector<string>&)>;

// Hàm split cơ bản
vector<string> split(const string& input) {
    istringstream iss(input);
    vector<string> tokens;
    string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

// Background Process Manager
class BackgroundManager {
private:
    static vector<thread> backgroundThreads;
    static int nextJobId;

public:
    // Thực thi lệnh nội bộ trong background
    static void executeInternalCommandBackground(const vector<string>& args, CommandFunction command) {
        int jobId = ++nextJobId;
        cout << "[Background Job " << jobId << "] Started" << endl;
        
        backgroundThreads.emplace_back([args, command, jobId]() {
            try {
                command(args);
                cout << "[Background Job " << jobId << "] Completed" << endl;
            } catch (const exception& e) {
                cerr << "[Background Job " << jobId << "] Error: " << e.what() << endl;
            }
        });
        
        // Detach thread để không cần join
        backgroundThreads.back().detach();
    }
    
    // Cleanup finished threads (optional - có thể gọi định kỳ)
    static void cleanupFinishedThreads() {
        // Note: Với detached threads, cleanup tự động xảy ra
        // Hàm này để mở rộng trong tương lai nếu cần quản lý threads tốt hơn
    }
};

// Static member definitions
vector<thread> BackgroundManager::backgroundThreads;
int BackgroundManager::nextJobId = 0;

// Command Wrapper Class
class CommandWrapper {
private:
    static unordered_map<string, CommandFunction> internalCommands;
    
public:
    // Khởi tạo command map
    static void initializeCommands() {
        internalCommands = {
            // Directory commands
            {"copy",           [&](auto& a) { directoryManager.copyDirectory(a); }},
            {"create_dir",     [&](auto& a) { directoryManager.createDirectory(a); }},
            {"delete_dir",     [&](auto& a) { directoryManager.deleteDirectory(a); }},
            {"list_dir_tree",  [&](auto& a) { directoryManager.listDirectoryTree(a); }},
            {"move_dir",       [&](auto& a) { directoryManager.moveDirectory(a); }},
            {"change_dir",     [&](auto& a) { directoryManager.changeDirectory(a); }},

            // File commands
            {"create_file",    [&](auto& a) { fileManager.createFile(a); }},
            {"delete_file",    [&](auto& a) { fileManager.deleteFile(a); }},
            {"copy_file",      [&](auto& a) { fileManager.copyFile(a); }},
            {"move_file",      [&](auto& a) { fileManager.moveFile(a); }},
            {"list_files",     [&](auto& a) { fileManager.listFiles(a); }},

            // Screen commands
            {"write_to_screen",  [&](auto& a) { screenManager.writeToScreen(a); }},
            {"read_file",        [&](auto& a) { screenManager.readFile(a); }},
            {"clear_terminal",   [&](auto&)   { screenManager.clearTerminal(); }}, 

            // Utility commands
            {"sliip",          [&](auto& a) { TimeManager::sleepShell(a); }},
            {"date",           [&](auto&)   { TimeManager::printDateTime(); }},
            {"prime",          [&](auto& a) { PrimeCounter::countPrimes(a); }},
            {"sysinfo",        [&](auto&)   { SystemInfoManager::showSystemInfo(); }},
            {"history",        [&](auto&)   { HistoryManager::showHistory(); }},
            {"taixiu",         [&](auto& a) { playTaiXiu(a); }},
            
            // Alias commands
            // {"alias_set",      [&](auto& a) { aliasManager.setAlias(a); }},
            // {"alias_list",     [&](auto&)   { aliasManager.listAliases(); }},
            // {"alias_remove",   [&](auto& a) { aliasManager.removeAlias(a); }},
        };
    }
    
    // Thực thi lệnh nội bộ (có hỗ trợ background)
    static bool executeCommand(const vector<string>& args, bool isBackground = false) {
        if (args.empty()) return false;
        
        auto it = internalCommands.find(args[0]);
        if (it == internalCommands.end()) {
            return false; // Không phải lệnh nội bộ
        }
        
        vector<string> subArgs(args.begin() + 1, args.end());
        
        if (isBackground) {
            // Thực thi trong background
            BackgroundManager::executeInternalCommandBackground(subArgs, it->second);
        } else {
            // Thực thi bình thường
            try {
                it->second(subArgs);
            } catch (const exception& e) {
                cerr << "Command error: " << e.what() << endl;
            }
        }
        
        return true;
    }
    
    // Kiểm tra xem có phải lệnh nội bộ không
    static bool isInternalCommand(const string& command) {
        return internalCommands.find(command) != internalCommands.end();
    }
    
    // Lấy danh sách tất cả lệnh nội bộ
    static vector<string> getInternalCommands() {
        vector<string> commands;
        for (const auto& pair : internalCommands) {
            commands.push_back(pair.first);
        }
        return commands;
    }
    
    // Thêm lệnh mới vào command map
    static void addCommand(const string& name, CommandFunction function) {
        internalCommands[name] = function;
    }
    
    // Xóa lệnh khỏi command map
    static void removeCommand(const string& name) {
        internalCommands.erase(name);
    }
};

// Static member definition
unordered_map<string, CommandFunction> CommandWrapper::internalCommands;

// Thực thi lệnh hệ thống bằng fork() và execvp()
void runExternalCommand(const vector<string>& args, bool isBackground) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    } else if (pid == 0) {
        // Child process
        vector<char*> cargs;
        for (const auto& arg : args) cargs.push_back(const_cast<char*>(arg.c_str()));
        cargs.push_back(nullptr);

        if (execvp(cargs[0], cargs.data()) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        if (!isBackground) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            cout << "[Background PID] " << pid << endl;
        }
    }
}

// Hàm hiển thị help
void showHelp() {
    cout << "Available internal commands:" << endl;
    auto commands = CommandWrapper::getInternalCommands();
    for (const auto& cmd : commands) {
        cout << "  " << cmd << endl;
    }
    cout << "\nUse '&' at the end of any command to run it in background" << endl;
    cout << "Example: prime 1000000 &" << endl;
}

// Chương trình chính
int main() {
    // Khởi tạo command wrapper
    CommandWrapper::initializeCommands();
    
    string input;
  //  cout << "Welcome to Enhanced Shell! Type 'help' for available commands." << endl;
    
    while (true) {
        cout << "$ ";
        getline(cin, input);
        if (cin.eof()) break;
        if (cin.fail()){
            cout << "Error reading input" << endl;
            cin.clear();
            continue;
        }
        if (input.empty()) continue;

        // Hỗ trợ alias
        input = aliasManager.resolveAlias(input);

        vector<string> args = split(input);
        if (args.empty()) continue;
        
        // Xử lý lệnh đặc biệt
        if (args[0] == "exit") break;
        if (args[0] == "help") {
            showHelp();
            continue;
        }

        // Kiểm tra nếu có & ở cuối -> chạy nền
        bool isBackground = false;
        if (!args.empty() && args.back() == "&") {
            isBackground = true;
            args.pop_back();
        }
        
        if (args.empty()) continue;

        // Thử thực thi lệnh nội bộ trước
        if (!CommandWrapper::executeCommand(args, isBackground)) {
            // Nếu không phải lệnh nội bộ, thực thi lệnh hệ thống
            runExternalCommand(args, isBackground);
        }
        
        // Cleanup threads định kỳ (optional)
        BackgroundManager::cleanupFinishedThreads();
    }
    
    cout << "Shell exited. Goodbye!" << endl;
    return 0;
}