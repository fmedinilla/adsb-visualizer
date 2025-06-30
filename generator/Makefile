# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -I../shared -g

# Directories
SRCDIR = src
BINDIR = bin
OBJDIR = build

# Source and object files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

SHARED_SOURCES = $(wildcard ../shared/*.c)
SHARED_OBJECTS = $(patsubst ../shared/%.c, $(OBJDIR)/%.o, $(SHARED_SOURCES))

# Target executable
TARGET = $(BINDIR)/generator

# Default target
all: $(TARGET)

# Link objects to create executable
$(TARGET): $(OBJECTS) $(SHARED_OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

# Compile source files to objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: ../shared/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if not exist
$(BINDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

# Run generator app
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
