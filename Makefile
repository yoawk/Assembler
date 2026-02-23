# [ Variables ]

# Compiler
CC := gcc

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build

# Compilation flags
BASE_CFLAGS:= -Wall -ansi -pedantic -I $(INCLUDE_DIR)
RELEASE_CFLAGS := $(BASE_CFLAGS)
DEBUG_CFLAGS := $(BASE_CFLAGS) -g -O0

# Define source files, their corresponding object files and executable
SRCS := $(wildcard $(SRC_DIR)/*.c) # Detects all .c files in SRC_DIR
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS)) # Sets corresponding .o files
TARGET := $(BUILD_DIR)/assembler

# [ Build targets ]

# Default target (release)
all: release

release: CFLAGS := $(RELEASE_CFLAGS)
release: clean-all
debug: $(TARGET)

debug: CFLAGS := $(DEBUG_CFLAGS)
debug: clean-all
debug: $(TARGET)

# [ Linking ]

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


# [ Compilation ]

# Rule to compile .c and .h files into .o
# Dependencies are the matching source and header files
# -c is used to compile the files without linking
# $< is the first dependency, the .c source file, and $@ is the target, .o file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# [ Cleanup ]

# Clean rule for deleting object files only
clean:
	rm -f $(OBJS)
# Clean rule for deleting executable and object files
clean-all:
	rm -f $(TARGET) $(OBJS)

.PHONY: all release debug clean clean-all
