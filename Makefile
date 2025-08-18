# Compiler and flags
CC      := gcc
CFLAGS  := -g -Wall -Wextra -O0 -MMD -MP
LDFLAGS :=

# Directories
SRC_DIR := src
OBJ_DIR := build
BIN_DIR := build

# Target name
TARGET  := $(BIN_DIR)/bf

# Source and object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Default target
all: $(TARGET)

# Link object files into final binary
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
