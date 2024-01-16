.PHONY: clean

STATIC_BUILD ?= 0

EXE_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
INCL_DIR = src
EXEC = $(EXE_DIR)/shadertest
DIST = $(EXE_DIR)/ShaderTest.zip
WARNS = -Wall -Wextra -Werror -Wpedantic -pedantic-errors
CXX_STD = -std=c++20
DEFINES =

ifeq ($(STATIC_BUILD), 1)
GLFW_PREFIX ?= /usr/local/src/glfw
INCL_GLFW_DIR = $(GLFW_PREFIX)/include
LIB_GLFW_DIR = $(GLFW_PREFIX)/build/release/src
LIB_GLFW = -L"$(LIB_GLFW_DIR)" -l:libglfw3.a
LIB_INCLUDES = -I"$(INCL_GLFW_DIR)"
else
LIB_GLFW = $$(pkg-config --libs glfw3)
LIB_INCLUDES =
endif
LIB_OPENGLES = $$(pkg-config --libs glesv2)
LIBS = $(LIB_GLFW) $(LIB_OPENGLES)

release: DEFINES =
release: OPTIMIZE = -O3
release: $(EXEC)

debug: DEFINES = -DDEBUG -g
debug: OPTIMIZE = -Og
debug: $(EXEC)

dist: $(EXEC)
	zip $(DIST) $(EXEC)

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
	rm -rf $(EXE_DIR) $(OBJ_DIR)
