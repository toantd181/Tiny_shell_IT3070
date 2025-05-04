#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include "Features/features.h"
#include "Processes/processes.h"

using namespace std;

DirectoryManager directoryManager;
FileManager fileManager;
ScreenManager screenManager;
AliasManager aliasManager;

void executeCommand(const string &command, const vector<string> &args){
    if (command == "copy"){
        directoryManager.copyDirectory(args);
    }

    else if (command == "create_dir"){
        directoryManager.createDirectory(args);
    }

    else if (command == "delete_dir"){
        directoryManager.deleteDirectory(args);
    }

    else if (command == "list_dir_tree"){
        directoryManager.listDirectoryTree(args);
    }

    else if (command == "move_dir"){
        directoryManager.moveDirectory(args);
    }

    else if (command == "change_dir"){
        directoryManager.changeDirectory(args);
    }

    else if (command == "create_file"){
        fileManager.createFile(args);
    }

    else if (command == "delete_file"){
        fileManager.deleteFile(args);
    }

    else if (command == "copy_file") {
        fileManager.copyFile(args);
    }

    else if (command == "move_file"){
        fileManager.moveFile(args);
    }

    else if (command == "list_files"){
        fileManager.listFiles(args);
    }

    else if (command == "taixiu"){
        playTaiXiu(args);
    }

    else if (command == "sysinfo") {
        SystemInfoManager::showSystemInfo();
    }

    else if (command == "history"){
        HistoryManager::showHistory();
    }

    else if (command == "sleep"){
        TimeManager::sleepShell(args);
    }

    else if (command == "date"){
        TimeManager::printDateTime();
    }

    else if (command == "prime"){
        PrimeCounter::countPrimes(args);
    }
}

vector<string> splitInput(const string &input){
    vector<string> tokens;
    string token;
    bool inQuotes = false;

    for (char ch : input)
    {
        if (ch == '\"')
        {
            inQuotes = !inQuotes;
            if (!inQuotes) // End of quoted string
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if (isspace(ch) && !inQuotes)
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if ((ch == '(' || ch == ')' || ch == ',' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '<' || ch == '>') && !inQuotes)
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(std::string(1, ch));
        }
        else
        {
            token += ch;
        }
    }

    if (!token.empty())
    {
        tokens.push_back(token);
    }

    if (!tokens.empty())
    {
        tokens[0] = aliasManager.resolveAlias(tokens[0]);
    }

    return tokens;
}

int main(){
    string input;
    while(true){
        cout << "tiny_shell [" << fs::current_path().string() << "]> ";
        getline(cin, input);
        HistoryManager::addCommand(input);
        if (cin.fail() || cin.eof()){
            cin.clear();
            cout << endl;
            continue;
        }

        vector<string> tokens = splitInput(input);
        if (tokens.empty()){
            continue;
        }

        string command = tokens[0];
        tokens.erase(tokens.begin());
        executeCommand(command, tokens);
    }
}
