#pragma once
#include <string>
#include <vector>
#include <functional>

namespace CommandWrapper {

    // Kiểu của hàm xử lý một lệnh: chỉ nhận vector<string> (đã split) và không trả về gì
    using CommandFunction = std::function<void(const std::vector<std::string>&)>;

    // Khởi tạo (xóa map cũ nếu có)
    void initialize();

    // Đăng ký một lệnh nội bộ
    void registerCommand(const std::string& name, CommandFunction cmd);

    // Thực thi lệnh đã split; nếu isBackground=true, chạy trong thread tách biệt
    // Trả về true nếu là lệnh nội bộ, false nếu không tìm thấy (sẽ báo lỗi ngoài)
    bool executeCommand(const std::vector<std::string>& args, bool isBackground);

    // Lấy danh sách các lệnh đã đăng ký (ví dụ dùng cho help)
    std::vector<std::string> listCommands();
}
