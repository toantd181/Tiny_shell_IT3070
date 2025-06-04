#include "scriptRunner.h"

#include "historyManager.h"
#include "../Builtin/builtins.h"
#include "../CommandWrapper/commandWrapper.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

using namespace std;
using namespace CommandWrapper;

/// Hàm phụ: tách một dòng thành tokens, hỗ trợ nháy đơn và nháy kép
static vector<string> tokenize_line(const string& line) {
    vector<string> tokens;
    string cur;
    bool inDouble = false, inSingle = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inDouble) {
            if (c == '"') {
                inDouble = false;
            } else if (c == '\\' && i + 1 < line.size()) {
                cur += line[++i];
            } else {
                cur += c;
            }
        }
        else if (inSingle) {
            if (c == '\'') {
                inSingle = false;
            } else {
                cur += c;
            }
        }
        else {
            if (isspace(static_cast<unsigned char>(c))) {
                if (!cur.empty()) {
                    tokens.push_back(cur);
                    cur.clear();
                }
            }
            else if (c == '"') {
                inDouble = true;
            }
            else if (c == '\'') {
                inSingle = true;
            }
            else {
                cur += c;
            }
        }
    }
    if (!cur.empty()) {
        tokens.push_back(cur);
    }
    return tokens;
}

/// Hàm chính: thực thi một script (mỗi dòng là một lệnh của tiny_shell)
void ScriptRunner::run_sh(const vector<string>& args) {
    if (args.empty()) {
        cerr << "Usage: run_sh <script_file>\n";
        return;
    }
    ifstream infile(args[0]);
    if (!infile) {
        cerr << "run_sh: cannot open script file: " << args[0] << "\n";
        return;
    }

    string raw;
    while (getline(infile, raw)) {
        // Bỏ qua whitespace đầu-cuối
        size_t start = raw.find_first_not_of(" \t\r\n");
        if (start == string::npos) continue;
        size_t end = raw.find_last_not_of(" \t\r\n");
        string line = raw.substr(start, end - start + 1);

        if (line.empty() || line[0] == '#') {
            continue;  // comment hoặc dòng trống
        }

        // Tách token
        vector<string> tokens = tokenize_line(line);
        if (tokens.empty()) continue;

        // Kiểm tra xem có & ở cuối (background) không
        bool isBackground = false;
        if (tokens.back() == "&") {
            isBackground = true;
            tokens.pop_back();
            if (tokens.empty()) continue;
        }

        // Ghi lịch sử (HistoryManager có sẵn)
        HistoryManager::addCommand(line);

        // Thử built-in trước
        if (Builtins::execute(tokens)) {
            continue;
        }
        // Thử lệnh do CommandWrapper cung cấp
        if (CommandWrapper::executeCommand(tokens, isBackground)) {
            continue;
        }

        // Nếu vẫn không tìm thấy lệnh
        cerr << "run_sh: unknown command: " << tokens[0] << "\n";
    }

    infile.close();
}
