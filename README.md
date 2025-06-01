# Tiny_Shell

Một shell đơn giản chạy trên Linux, hỗ trợ:
- Phân tích cú pháp (tokenize) với dấu nháy và escape.
- Chạy lệnh foreground (shell chờ) và background (shell không chờ).
- Quản lý tiến trình nền (jobs): `ps`, `kill`, `stop`, `resume`.
- Các lệnh nội bộ (built-in): `help`, `exit`/`quit`, `date`, `history`, `clear_history`.
- Tương tác với hệ thống file/thư mục: `create_file`, `delete_file`, `copy_file`, `move_file`, `list_files`, `create_dir`, `delete_dir`, `list_dir_tree`, `move_dir`, `change_dir`.
- Hiển thị thông tin hệ thống (`sysinfo`), sleep (`sliip`), chạy file Assembly (`run_s <tệp.s>`), batch cơ bản (`run_bat <tệp.bat>`).
- ShellPath riêng (các thư mục tìm kiếm nhị phân do người dùng thêm).

---

## Yêu cầu (Prerequisites)

- **Hệ điều hành**: Linux (Ubuntu, Fedora, CentOS, v.v.).  
- **Trình biên dịch**: `g++` (phiên bản ≥ 7.0, hỗ trợ C++17).  
- **Make**: đã cài sẵn `make`.

---

## Cài đặt (Build)

1. Git clone về máy:
   ```bash
   git clone https://github.com/toantd181/Tiny_shell_IT3070.git
   cd Tiny_shell_IT3070
   ```
2. Biên dịch bằng make:
   ``` make
   ```
   Kết quả sẽ sinh ra file thực thi `tiny_shell` trong thư mục gốc dự án
3. Chạy shell:
   ```./tiny_shell
   ```
   Giao diện prompt sẽ hiển thị:
   ```tiny_shell>
   ``` 
