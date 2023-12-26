.PHONY: clean

EXE_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
INCL_DIR = src
EXEC = $(EXE_DIR)/shadertest
DIST = $(EXE_DIR)/ShaderTest.tar.gz
WARNS = -Wall -Wextra -Werror -Wpedantic -pedantic-errors
CXX_STD = -std=c++20
DEFINES =

GLFW_PREFIX ?= /usr/local/src/glfw
INCL_GLFW_DIR = $(GLFW_PREFIX)/include
LIB_GLFW_DIR = $(GLFW_PREFIX)/build/release/src
LIB_GLFW = -L"$(LIB_GLFW_DIR)" -l:libglfw3.a

GLEW_PREFIX ?= /usr/local/src/glew
INCL_GLEW_DIR = $(GLEW_PREFIX)/include
LIB_GLEW_DIR = $(GLEW_PREFIX)/lib
LIB_GLEW = -L"$(LIB_GLEW_DIR)" -l:libGLEW.a

LIB_OPENGL = $$(pkg-config --libs gl)

LIB_INCLUDES = -I"$(INCL_GLFW_DIR)" -I"$(INCL_GLEW_DIR)"
LIBS = $(LIB_GLFW) $(LIB_GLEW) $(LIB_OPENGL)

release: OPTIMIZE = -O3
release: $(EXEC)

debug: DEFINES = $(DEFINES) -DDEBUG -g
debug: OPTIMIZE = -Og
debug: $(EXEC)

dist: $(EXEC)
	tar -c -f $(DIST) -v -z $(EXEC)

COMPATIBILITY_HH = $(INCL_DIR)/compatibility.hh
CONFIGURATIONS_HH = $(INCL_DIR)/configurations.hh
CORE_HH = $(INCL_DIR)/core.hh
CORE_CC = $(SRC_DIR)/core.cc
CORE_OBJ = $(OBJ_DIR)/core.o
DEBUG_HH = $(SRC_DIR)/debug.hh
GEOMETRY_HH = $(INCL_DIR)/geometry.hh
GL_INCLUDES_HH = $(INCL_DIR)/gl-includes.hh
MAIN_CC = $(SRC_DIR)/main.cc
MAIN_OBJ = $(OBJ_DIR)/main.o
RUNTIME_HH = $(INCL_DIR)/runtime.hh
RUNTIME_CC = $(SRC_DIR)/runtime.cc
RUNTIME_OBJ = $(OBJ_DIR)/runtime.o
WINDOW_HH = $(INCL_DIR)/window.hh
WINDOW_CC = $(SRC_DIR)/window.cc
WINDOW_OBJ = $(OBJ_DIR)/window.o

$(EXEC): $(MAIN_OBJ) $(CORE_OBJ) $(RUNTIME_OBJ) $(WINDOW_OBJ)
	mkdir -p $(EXE_DIR)
	$(CXX) -o $@ $^ $(LIBS)

$(MAIN_OBJ): $(MAIN_CC) $(COMPATIBILITY_HH) $(CONFIGURATIONS_HH) $(CORE_HH) $(RUNTIME_HH) $(WINDOW_HH) $(DEBUG_HH)
	mkdir -p $(OBJ_DIR)
	$(CXX) -c -o $@ $< $(WARNS) $(DEFINES) $(OPTIMIZE) $(CXX_STD) $(LIB_INCLUDES)

$(CORE_OBJ): $(CORE_CC) $(CORE_HH) $(CONFIGURATIONS_HH) $(GL_INCLUDES_HH) $(DEBUG_HH)
	$(CXX) -c -o $@ $< $(WARNS) $(DEFINES) $(OPTIMIZE) $(CXX_STD) $(LIB_INCLUDES)

$(RUNTIME_OBJ): $(RUNTIME_CC) $(RUNTIME_HH) $(CONFIGURATIONS_HH) $(CORE_HH) $(GEOMETRY_HH) $(GL_INCLUDES_HH) $(DEBUG_HH)
	$(CXX) -c -o $@ $< $(WARNS) $(DEFINES) $(OPTIMIZE) $(CXX_STD) $(LIB_INCLUDES)

$(WINDOW_OBJ): $(WINDOW_CC) $(WINDOW_HH) $(COMPATIBILITY_HH) $(CONFIGURATIONS_HH) $(GL_INCLUDES_HH) $(DEBUG_HH)
	$(CXX) -c -o $@ $< $(WARNS) $(DEFINES) $(OPTIMIZE) $(CXX_STD) $(LIB_INCLUDES)

clean:
	rm -rf $(EXE_DIR)
	rm -rf $(OBJ_DIR)
