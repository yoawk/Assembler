# [ Variables ]

# Compiler
CC := gcc

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build

RELEASE_DIR := $(BUILD_DIR)/release
DEBUG_DIR := $(BUILD_DIR)/debug

# Compilation flags
BASE_CFLAGS:= -Wall -ansi -pedantic -I $(INCLUDE_DIR)
RELEASE_CFLAGS := $(BASE_CFLAGS)
DEBUG_CFLAGS := $(BASE_CFLAGS) -g -O0

# Define source files, their corresponding object files and executable
SRCS := $(wildcard $(SRC_DIR)/*.c)

RELEASE_OBJS := $(patsubst $(SRC_DIR)/%.c,$(RELEASE_DIR)/%.o,$(SRCS))
DEBUG_OBJS := $(patsubst $(SRC_DIR)/%.c,$(DEBUG_DIR)/%.o,$(SRCS))

RELEASE_TARGET := $(RELEASE_DIR)/assembler
DEBUG_TARGET := $(DEBUG_DIR)/assembler

# [ Build targets ]

# Default target (release)
all: release

release: $(RELEASE_TARGET)

debug: $(DEBUG_TARGET)

# [ Linking ]

$(RELEASE_TARGET): $(RELEASE_OBJS)
	$(CC) $(RELEASE_CFLAGS) -o $(RELEASE_TARGET) $(RELEASE_OBJS)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CC) $(DEBUG_CFLAGS) -o $(DEBUG_TARGET) $(DEBUG_OBJS)


# [ Compilation ]

# Rule to compile .c and .h files into .o
# Dependencies are the matching source and header files
# -c is used to compile the files without linking
# $< is the first dependency, the .c source file, and $@ is the target, .o file

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.c | $(RELEASE_DIR)
	$(CC) $(RELEASE_CFLAGS) -c $< -o $@

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c | $(DEBUG_DIR)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

# [ Build Directories ]
$(RELEASE_DIR):
	mkdir -p $(RELEASE_DIR)

$(DEBUG_DIR):
	mkdir -p $(DEBUG_DIR)

# [ Cleanup ]

# Clean rule for deleting object files only
clean:
	rm -f $(OBJS)

# Clean rule for deleting all build files
clean-all:
	rm -rf $(BUILD_DIR)

.PHONY: all release debug clean clean-all
