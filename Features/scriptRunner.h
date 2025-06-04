#ifndef SCRIPT_RUNNER_H
#define SCRIPT_RUNNER_H

#include <vector>
#include <string>

namespace ScriptRunner {
    /// Đọc file script, chạy tuần tự từng lệnh (hỗ trợ &)
    void run_sh(const std::vector<std::string>& args);
}

#endif // SCRIPT_RUNNER_H
