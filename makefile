.PHONY: clean

EXE_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
INCL_DIR = src
EXEC = $(EXE_DIR)/shadertest
WARNS = -Wall -Wextra -Werror -Wpedantic -pedantic-errors
CXX_STD = -std=c++20
DEBUGS =
LIBS = -lGL -lGLEW -lglfw

release: DEBUGS = -O3
release: $(EXEC)

debug: DEBUGS = -DDEBUG -Og -g
debug: $(EXEC)

COMPATIBILITY_HH = $(INCL_DIR)/compatibility.hh
CONFIGURATIONS_HH = $(INCL_DIR)/configurations.hh
CORE_HH = $(INCL_DIR)/core.hh
CORE_CC = $(SRC_DIR)/core.cc
CORE_OBJ = $(OBJ_DIR)/core.o
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

$(MAIN_OBJ): $(MAIN_CC) $(COMPATIBILITY_HH) $(CONFIGURATIONS_HH) $(CORE_HH) $(RUNTIME_HH) $(WINDOW_HH)
	mkdir -p $(OBJ_DIR)
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS) $(CXX_STD)

$(CORE_OBJ): $(CORE_CC) $(CORE_HH) $(CONFIGURATIONS_HH) $(GL_INCLUDES_HH)
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS) $(CXX_STD)

$(RUNTIME_OBJ): $(RUNTIME_CC) $(RUNTIME_HH) $(CONFIGURATIONS_HH) $(CORE_HH) $(GEOMETRY_HH) $(GL_INCLUDES_HH)
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS) $(CXX_STD)

$(WINDOW_OBJ): $(WINDOW_CC) $(WINDOW_HH) $(COMPATIBILITY_HH) $(CONFIGURATIONS_HH) $(GL_INCLUDES_HH)
	$(CXX) -c -o $@ $< $(WARNS) $(DEBUGS) $(CXX_STD)

clean:
	rm -rf $(EXE_DIR)
	rm -rf $(OBJ_DIR)
