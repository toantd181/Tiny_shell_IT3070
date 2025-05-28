#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_set>

using namespace std;
namespace fs = std::filesystem;

class ScreenManager {
public:
    static const unordered_set<string> supportedCommands;
    static const unordered_set<string>& getSupportedCommands();

    static void writeToScreen(const vector<string>& args) {
        if (args.empty()) {
            printUsage("write_to_screen <text> [> <file_path>]");
            return;
        }

        bool writeToFile = (args.size() > 2 && args[args.size() - 2] == ">");
        fs::path filePath;
        if (writeToFile) filePath = args.back();

        ofstream file;
        if (writeToFile) file.open(filePath, ios::app);

        for (size_t i = 0; i < args.size() - (writeToFile ? 2 : 0); ++i) {
            if (writeToFile) file << args[i] << " ";
            else cout << args[i] << " ";
        }
        cout << endl;
        if (writeToFile) file << endl;
    }

    static void readFile(const vector<string>& args) {
        if (args.size() != 1) {
            printUsage("read_file <file_path>");
            return;
        }

        fs::path filePath = args[0];
        if (!fs::exists(filePath)) {
            cerr << "File does not exists: " << filePath << endl;
            return;
        }

        ifstream file(filePath);
        if (!file) {
            cerr << "Failed to open file: " << filePath << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
    }

    static void clearTerminal() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

private:
    static void printUsage(const string &usage) {
        cerr << "Usage: " << usage << endl;
    }
};

#endif
