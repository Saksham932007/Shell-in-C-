# Makefile for C++ Shell

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = shell
SOURCE = shell.cpp

# Default target
all: $(TARGET)

# Build the shell executable
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Install the shell (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall the shell (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Run the shell
run: $(TARGET)
	./$(TARGET)

# Debug build with extra debugging symbols
debug: CXXFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Release build with optimizations
release: CXXFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)

.PHONY: all clean install uninstall run debug release
