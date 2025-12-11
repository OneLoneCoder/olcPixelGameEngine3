# OneLoneCoder Pixel Game Engine 3 (olcPGE3) Universal Binary Makefile
# This makefile builds the PGE3 library and test executable for both ARM64 and x86_64 architectures on macOS

# Compiler and flags
CXX = clang++
CXXFLAGS_COMMON = -std=c++20 -Wall -Wextra -O2 -DGL_SILENCE_DEPRECATION
DEBUG_CXXFLAGS_COMMON = -std=c++20 -Wall -Wextra -g -DGL_SILENCE_DEPRECATION -DDEBUG

# Architecture-specific flags
ARM64_FLAGS = -arch arm64
X86_64_FLAGS = -arch x86_64
UNIVERSAL_FLAGS = -arch arm64 -arch x86_64

# Final flags
CXXFLAGS_ARM64 = $(CXXFLAGS_COMMON) $(ARM64_FLAGS)
CXXFLAGS_X86_64 = $(CXXFLAGS_COMMON) $(X86_64_FLAGS)
CXXFLAGS_UNIVERSAL = $(CXXFLAGS_COMMON) $(UNIVERSAL_FLAGS)
DEBUG_CXXFLAGS_ARM64 = $(DEBUG_CXXFLAGS_COMMON) $(ARM64_FLAGS)
DEBUG_CXXFLAGS_X86_64 = $(DEBUG_CXXFLAGS_COMMON) $(X86_64_FLAGS)
DEBUG_CXXFLAGS_UNIVERSAL = $(DEBUG_CXXFLAGS_COMMON) $(UNIVERSAL_FLAGS)

# Directories
SRC_DIR = ../src
TEST_DIR = ../tests
BIN_DIR = bin
BUILD_DIR = build
BUILD_ARM64_DIR = build/arm64
BUILD_X86_64_DIR = build/x86_64
BUILD_DEBUG_DIR = build/debug

# Include directories
INCLUDES = -I$(SRC_DIR)

# System libraries (macOS specific)
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lobjc

# Source files - platform-specific selection
CORE_SOURCES = $(SRC_DIR)/core.cpp \
               $(SRC_DIR)/window.cpp \
               $(SRC_DIR)/image.cpp \
               $(SRC_DIR)/draw2d.cpp \
               $(SRC_DIR)/hw_mouse.cpp \
               $(SRC_DIR)/gpu_iface.cpp \
               $(SRC_DIR)/gpu_opengl33.cpp \
               $(SRC_DIR)/api_opengl.cpp

# Platform-specific sources for macOS
PLATFORM_SOURCES = $(SRC_DIR)/api_macos.cpp \
                   $(SRC_DIR)/host_apple_macos.cpp \
                   $(SRC_DIR)/imload_macos.cpp

# All library sources
LIB_SOURCES = $(CORE_SOURCES) $(PLATFORM_SOURCES)

# Test source files
TEST_MAIN_SOURCE = $(TEST_DIR)/test_mh.cpp

# All sources for final executable
ALL_SOURCES = $(LIB_SOURCES) $(TEST_MAIN_SOURCE)

# Object files for different architectures
ARM64_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_ARM64_DIR)/%.o,$(LIB_SOURCES)) \
                $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_ARM64_DIR)/%.o,$(TEST_MAIN_SOURCE))

X86_64_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_X86_64_DIR)/%.o,$(LIB_SOURCES)) \
                 $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_X86_64_DIR)/%.o,$(TEST_MAIN_SOURCE))

# Debug object files
DEBUG_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DEBUG_DIR)/%.o,$(LIB_SOURCES)) \
                $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DEBUG_DIR)/%.o,$(TEST_MAIN_SOURCE))

# Target names
TEST_EXECUTABLE = test_mh
ARM64_EXECUTABLE = $(BUILD_ARM64_DIR)/$(TEST_EXECUTABLE)
X86_64_EXECUTABLE = $(BUILD_X86_64_DIR)/$(TEST_EXECUTABLE)
UNIVERSAL_EXECUTABLE = $(BIN_DIR)/$(TEST_EXECUTABLE)
DEBUG_EXECUTABLE = $(BUILD_DEBUG_DIR)/$(TEST_EXECUTABLE)

# Phony targets
.PHONY: all arm64 x86_64 universal debug clean test run install help info check-arch

# Default target - build universal binary
all: universal

# Help target
help:
	@echo "OneLoneCoder Pixel Game Engine 3 Universal Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build universal binary (default)"
	@echo "  universal  - Build universal binary (ARM64 + x86_64)"
	@echo "  arm64      - Build ARM64 binary only"
	@echo "  x86_64     - Build x86_64 binary only"
	@echo "  debug      - Build debug version"
	@echo "  test       - Build and run the executable"
	@echo "  run        - Run the executable (build if needed)"
	@echo "  clean      - Remove all build files"
	@echo "  install    - Install executable (requires sudo)"
	@echo "  info       - Show build information"
	@echo "  check-arch - Check architectures in built executable"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Build configurations:"
	@echo "  Universal: make all (or make universal)"
	@echo "  ARM64:     make arm64"
	@echo "  x86_64:    make x86_64"
	@echo "  Debug:     make debug"

# Create build directories
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_ARM64_DIR):
	@mkdir -p $(BUILD_ARM64_DIR)

$(BUILD_X86_64_DIR):
	@mkdir -p $(BUILD_X86_64_DIR)

$(BUILD_DEBUG_DIR):
	@mkdir -p $(BUILD_DEBUG_DIR)

# ARM64 executable
$(ARM64_EXECUTABLE): $(BUILD_ARM64_DIR) $(ARM64_OBJECTS)
	@echo "Linking ARM64 executable: $@"
	$(CXX) $(CXXFLAGS_ARM64) -o $@ $(ARM64_OBJECTS) $(LIBS)
	@echo "ARM64 executable created successfully!"

# x86_64 executable
$(X86_64_EXECUTABLE): $(BUILD_X86_64_DIR) $(X86_64_OBJECTS)
	@echo "Linking x86_64 executable: $@"
	$(CXX) $(CXXFLAGS_X86_64) -o $@ $(X86_64_OBJECTS) $(LIBS)
	@echo "x86_64 executable created successfully!"

# Universal binary (lipo both architectures together)
$(UNIVERSAL_EXECUTABLE): $(ARM64_EXECUTABLE) $(X86_64_EXECUTABLE) $(BIN_DIR)
	@echo "Creating universal binary: $@"
	lipo -create -output $@ $(ARM64_EXECUTABLE) $(X86_64_EXECUTABLE)
	@echo "Universal binary created successfully!"

# Debug executable
$(DEBUG_EXECUTABLE): $(BUILD_DEBUG_DIR) $(DEBUG_OBJECTS)
	@echo "Linking debug executable: $@"
	$(CXX) $(DEBUG_CXXFLAGS_UNIVERSAL) -o $@ $(DEBUG_OBJECTS) $(LIBS)
	@echo "Debug executable created successfully!"

# Compile source files for ARM64
$(BUILD_ARM64_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_ARM64_DIR)
	@echo "Compiling (ARM64): $<"
	$(CXX) $(CXXFLAGS_ARM64) $(INCLUDES) -c $< -o $@

$(BUILD_ARM64_DIR)/%.o: $(TEST_DIR)/%.cpp $(BUILD_ARM64_DIR)
	@echo "Compiling test (ARM64): $<"
	$(CXX) $(CXXFLAGS_ARM64) $(INCLUDES) -c $< -o $@

# Compile source files for x86_64
$(BUILD_X86_64_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_X86_64_DIR)
	@echo "Compiling (x86_64): $<"
	$(CXX) $(CXXFLAGS_X86_64) $(INCLUDES) -c $< -o $@

$(BUILD_X86_64_DIR)/%.o: $(TEST_DIR)/%.cpp $(BUILD_X86_64_DIR)
	@echo "Compiling test (x86_64): $<"
	$(CXX) $(CXXFLAGS_X86_64) $(INCLUDES) -c $< -o $@

# Compile source files for debug
$(BUILD_DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_DEBUG_DIR)
	@echo "Compiling (debug): $<"
	$(CXX) $(DEBUG_CXXFLAGS_UNIVERSAL) $(INCLUDES) -c $< -o $@

$(BUILD_DEBUG_DIR)/%.o: $(TEST_DIR)/%.cpp $(BUILD_DEBUG_DIR)
	@echo "Compiling test (debug): $<"
	$(CXX) $(DEBUG_CXXFLAGS_UNIVERSAL) $(INCLUDES) -c $< -o $@

# Architecture-specific targets
arm64: $(ARM64_EXECUTABLE)

x86_64: $(X86_64_EXECUTABLE)

universal: $(UNIVERSAL_EXECUTABLE)

debug: $(DEBUG_EXECUTABLE)

# Test and run targets
test: $(UNIVERSAL_EXECUTABLE)
	@echo "Running executable..."
	@cd $(BIN_DIR) && ./$(TEST_EXECUTABLE)

run: $(UNIVERSAL_EXECUTABLE)
	@echo "Running executable..."
	@cd $(BIN_DIR) && ./$(TEST_EXECUTABLE)

# Check architecture of built executable
check-arch: $(UNIVERSAL_EXECUTABLE)
	@echo "Architecture information for $(UNIVERSAL_EXECUTABLE):"
	@file $(UNIVERSAL_EXECUTABLE)
	@echo ""
	@echo "Detailed architecture info:"
	@lipo -info $(UNIVERSAL_EXECUTABLE)

# Install target (installs to /usr/local by default)
PREFIX ?= /usr/local
install: $(UNIVERSAL_EXECUTABLE)
	@echo "Installing olcPGE3 executable..."
	install -d $(PREFIX)/bin
	install -m 755 $(UNIVERSAL_EXECUTABLE) $(PREFIX)/bin/
	@echo "Installation complete!"
	@echo "Executable installed to: $(PREFIX)/bin/$(TEST_EXECUTABLE)"

# Clean target
clean:
	@echo "Cleaning build files and output..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete!"

# Show build information
info:
	@echo "Build Configuration:"
	@echo "  Compiler: $(CXX)"
	@echo "  Common Flags: $(CXXFLAGS_COMMON)"
	@echo "  ARM64 Flags: $(CXXFLAGS_ARM64)"
	@echo "  x86_64 Flags: $(CXXFLAGS_X86_64)"
	@echo "  Universal Flags: $(CXXFLAGS_UNIVERSAL)"
	@echo "  Debug Flags: $(DEBUG_CXXFLAGS_UNIVERSAL)"
	@echo "  Libraries: $(LIBS)"
	@echo "  Source Directory: $(SRC_DIR)"
	@echo "  Test Directory: $(TEST_DIR)"
	@echo "  Build Directory: $(BUILD_DIR)"
	@echo "  Output Directory: $(BIN_DIR)"
	@echo ""
	@echo "Library Sources:"
	@echo "$(LIB_SOURCES)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "Test Source:"
	@echo "  $(TEST_MAIN_SOURCE)"
	@echo ""
	@echo "Target Executables:"
	@echo "  Universal: $(UNIVERSAL_EXECUTABLE)"
	@echo "  ARM64: $(ARM64_EXECUTABLE)"
	@echo "  x86_64: $(X86_64_EXECUTABLE)"
	@echo "  Debug: $(DEBUG_EXECUTABLE)"
