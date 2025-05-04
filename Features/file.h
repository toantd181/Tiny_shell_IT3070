#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_set>

using namespace std;
namespace fs = std::filesystem;

class FileManager {
    public:
        static const unordered_set<string> supportedCommands;
        static const unordered_set<string>& getSupportedCommands() {
            return supportedCommands;
        }

        void createFile (const vector<string>& args){
            if (args.size() != 1){
                printUsage("create_file <file_path>");
                return;
            }

        fs::path filePath = args[0];
        ofstream file(filePath);
        if (file){
            cout << "File is created: " << filePath << endl;
        } else {
            cerr << "Error creating file: " << filePath << endl;
        }
    }

        void deleteFile (const vector<string>& args){
            if (args.size() != 1){
                printUsage("delete_file <file_path>");
                return;
            }

        fs::path filePath = args[0];
        if (!filesystem::exists(filePath)){
            cerr << "File does not exist: " << filePath << endl;
            return;
        }
        filesystem::remove(filePath);
        cout << "File deleted: " << filePath << endl;
    }

        void copyFile (const vector<string>& args){
            if (args.size() != 2){
                printUsage("copy_file <source_file> <destination_file>");
                return;
            }

        fs::path src = args[0];
        fs::path dst = args[1];
        if (!filesystem::exists(src)){
            cerr << "Source file does not exist: " << src << endl;
            return;
        }
        filesystem::copy(src, dst);
        cout << "File copied from " << src << " to " << dst << endl;
    }

        void moveFile (const vector<string>& args){
            if (args.size() != 2){
                printUsage("move_file <source_file> <destination_file>");
                return;
            }

        fs::path src = args[0];
        fs::path dst = args[1];
        if (!filesystem::exists(src)){
            cerr << "Source file does not exist: " << src << endl;
            return;
        }
        filesystem::rename(src, dst);
        cout << "File moved from " << src << " to " << dst << endl;
    }
/*
        void readFile (const vector<string>& args){
            if (args.size() != 1){
                printUsage("cat <file_path>");
                return;
            }
        fs::path filePath = args[0];
        if (!filesystem::exists(filePath)){
            cerr << "File does not exist: " << filePath << endl;
            return;
        }
        ifstream file(filePath);
        if (!file){
            cerr << "Error opening file: " << filePath << endl;
            return;
        }
        string line;
        while (getline(file, line)){
            cout << line << endl;
        }
        file.close();
    }
*/
        void listFiles(const vector<string>& args){
            fs::path dirPath;
            if (args.empty()){
                dirPath = ".";
            }
            else {
                dirPath = args[0];
            }
            if (!fs::exists(dirPath) || !fs::is_directory(dirPath)){
                cerr << "Directory does not exist: " << dirPath << endl;
                return;
            }
            for (const auto& entry : fs::directory_iterator(dirPath)){
                cout << entry.path().filename().string() << endl;
            }
            cout << endl;
        }

    private:
        static void printUsage(const std::string &usage)
            {
                std::cerr << "Usage: " << usage << std::endl;
            }
};

    const unordered_set<string> FileManager::supportedCommands = {
    "create_file",
    "delete_file",
    "copy_file",
    "move_file",
    "list_files"
};

#endif // FILE_MANAGER_H
