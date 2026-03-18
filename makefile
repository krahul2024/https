# makefile
.PHONY: all run clean

# directories
SRC_DIR := src
UTIL_DIR := utils
BUILD_DIR := build

# compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -I$(SRC_DIR) -I$(UTIL_DIR)

# find all .cc source files in src/
SRC := $(wildcard $(SRC_DIR)/*.cc)

# map source files to object files in build/
OBJ := $(patsubst $(SRC_DIR)/%.cc,$(BUILD_DIR)/%.o,$(SRC))

# default target
all: $(BUILD_DIR)/main

# link the executable
$(BUILD_DIR)/main: $(OBJ)
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(OBJ) -o $@

# compile .cc -> .o with dependency tracking
# -mmd generates .d files (dependencies)
# -mf specifies where to write the .d file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -MMD -MF $(BUILD_DIR)/$*.d -c $< -o $@

# include generated dependency files
-include $(OBJ:.o=.d)

# run the program
run: all
	@./$(BUILD_DIR)/main

# clean build artifacts
clean:
	@rm -rf $(BUILD_DIR)
