# [ Variables ]

# Compiler
CC := gcc

# Executable name
TARGET := assembler

# Compilation flags
BASE_CFLAGS:= -Wall -ansi -pedantic
RELEASE_CFLAGS := $(BASE_CFLAGS)
DEBUG_CFLAGS := $(BASE_CFLAGS) -g -O0

# Define source files and their corresponding object files
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

# [ Build targets ]

# Default target (release)
all: release

release: CFLAGS := $(RELEASE_CFLAGS)
release: clean $(TARGET)

debug: CFLAGS := $(DEBUG_CFLAGS)
debug: clean $(TARGET)

# [ Linking ]

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)


# [ Compilation ]

# Rule to compile .c and .h files into .o
# Dependencies are the matching source and header files
# -c is used to compile the files without linking
# $< is the first dependency, the .c source file, and $@ is the target, .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# [ Cleanup ]

# Clean rule for deleting object files only
clean:
	rm -f $(OBJS)
# Clean rule for deleting executable and object files
clean-all:
	rm -f $(TARGET) $(OBJS)

.PHONY: all release debug clean clean-all
