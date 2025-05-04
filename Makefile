CXX = g++
CXXFLAGS = -std=c++17

# Tên chương trình cuối cùng
TARGET = tiny_shell

# Các tệp nguồn thực tế (bỏ qua các tệp .h)
SRC = main.cpp Processes/taixiu.cpp Processes/primeCounter.cpp

# Các tệp đối tượng
OBJ = $(SRC:.cpp=.o)

# Quy tắc chính để tạo chương trình
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Quy tắc để tạo đối tượng .o từ .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Quy tắc làm sạch
clean:
	rm -f $(OBJ) $(TARGET)
