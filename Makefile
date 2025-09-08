CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude -I.
SRC_DIR := src
BUILD_DIR := build
OUT_DIR := output

# Collect all source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target: build both server and client
all: $(OUT_DIR)/server $(OUT_DIR)/client

# Compile each .cpp into .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link server (no QueuesFactory.o needed, it’s header-only)
$(OUT_DIR)/server: $(BUILD_DIR)/server.o $(BUILD_DIR)/jobs.o | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Link client
$(OUT_DIR)/client: $(BUILD_DIR)/client.o $(BUILD_DIR)/jobs.o | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Ensure directories exist
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)/server $(OUT_DIR)/client

.PHONY: all clean



OUT_TEST := $(OUT_DIR)/testQueue
SRCS_TEST := $(SRC_DIR)/mainTest.cpp
OBJS_TEST := $(SRCS_TEST:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

$(OUT_TEST): $(OBJS_TEST) | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile mainTest.cpp
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Link server
$(OUT_DIR)/server: $(BUILD_DIR)/server.o $(BUILD_DIR)/jobs.o | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ -ljsoncpp

# Link client
$(OUT_DIR)/client: $(BUILD_DIR)/client.o $(BUILD_DIR)/jobs.o | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ -ljsoncpp
