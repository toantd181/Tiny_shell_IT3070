CXX = g++
CXXFLAGS = -std=c++17 -Wall -I.

# Target executable
TARGET = tiny_shell

# Source files
SRC = main.cpp CommandWrapper/commandWrapper.cpp Builtin/builtins.cpp Processes/taixiu.cpp Processes/primeCounter.cpp


# Object files
OBJ = $(SRC:.cpp=.o)

# Main rule
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Rule for object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean
