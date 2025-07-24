# Compiler and Flags
CC := clang
CFLAGS_DEBUG := -fsanitize=address -g -O0 -Iinclude -Wall -Werror -ftime-trace
CFLAGS_RELEASE := -g -O3 -Iinclude

# Linking
LDFLAGS := -L/opt/homebrew/opt/curl/lib -lcurl

# Paths
SRC_DIR := ./src
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Targets
TARGET_DEBUG := $(BIN_DIR)/shazam_clone_d
TARGET_RELEASE := $(BIN_DIR)/shazam_clone

# Default target
all: $(TARGET_DEBUG)

# Debug Build
$(TARGET_DEBUG): CFLAGS := $(CFLAGS_DEBUG)
$(TARGET_DEBUG): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking $@ ..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Release Build
release: $(TARGET_RELEASE)

$(TARGET_RELEASE): CFLAGS := $(CFLAGS_RELEASE)
$(TARGET_RELEASE): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking $@ ..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Object compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)

