# Makefile for Bezier 3D Modeling Application

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -O2

# Source directory
SRCDIR = src

# Source files
SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/Aplicacao3D.cpp \
          $(SRCDIR)/CurvaBezier.cpp \
          $(SRCDIR)/Objeto3D.cpp \
          $(SRCDIR)/gl_canvas2d.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Target executable
TARGET = bezier3d

# Libraries (adjust for your system)
# Windows (MinGW)
ifeq ($(OS),Windows_NT)
    LIBS = -lopengl32 -lglu32 -lfreeglut
else
    # Linux/Unix
    LIBS = -lGL -lGLU -lglut
endif

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)
ifeq ($(OS),Windows_NT)
	del /Q src\*.o $(TARGET).exe 2>nul || true
endif

# Clean and rebuild
rebuild: clean all

# Install dependencies (Linux/Ubuntu)
install-deps:
	sudo apt-get update
	sudo apt-get install build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the application"
	@echo "  clean        - Remove build files"
	@echo "  rebuild      - Clean and build"
	@echo "  install-deps - Install dependencies (Linux only)"
	@echo "  help         - Show this help"

.PHONY: all clean rebuild install-deps help
