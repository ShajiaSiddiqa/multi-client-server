CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude -I.
LDFLAGS := 

SRC_DIR := src
OBJ_DIR := build
OUT_DIR := output

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# Object files (map src/*.cpp → build/*.o)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Main executable
TARGET := $(OUT_DIR)/main

# Rule to build everything
all: $(TARGET)

# Link step
$(TARGET): $(OBJS) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile each .cpp → build/*.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure directories exist
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)/main

.PHONY: all clean
