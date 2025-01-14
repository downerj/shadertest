.PHONY: clean

STATIC_BUILD ?= 0

EXECUTABLE_DIRECTORY = bin
OBJECT_DIRECTORY = obj
SOURCE_DIRECTORY = src
LIBRARY_DIRECTORY = lib
INCLUDE_DIRECTORY = include
SHADERS_DIRECTORY = shaders
EXECUTABLE = ${EXECUTABLE_DIRECTORY}/shadertest
DISTRIBUTABLE = ${EXECUTABLE_DIRECTORY}/ShaderTest.zip
WARNINGS = -Wall -Wextra -Wpedantic
CXX_STANDARD = -std=c++17

ifeq (${STATIC_BUILD}, 1)
GLFW_LIBRARY = -L"${LIBRARY_DIRECTORY}" -l:libglfw3.a
else
GLFW_LIBRARY = $$(pkg-config --libs glfw3)
endif
INCLUDES = -I${INCLUDE_DIRECTORY} ${GLFW_INCLUDE}
LIBRARIES = ${GLFW_LIBRARY}

DEBUG_HXX = ${SOURCE_DIRECTORY}/debug.hxx
GRAPHICS_CXX = ${SOURCE_DIRECTORY}/graphics.cxx
GRAPHICS_HXX = ${SOURCE_DIRECTORY}/graphics.hxx
GRAPHICS_OBJ = ${OBJECT_DIRECTORY}/graphics.o
IO_CXX = ${SOURCE_DIRECTORY}/io.cxx
IO_HXX = ${SOURCE_DIRECTORY}/io.hxx
IO_OBJ = ${OBJECT_DIRECTORY}/io.o
MAIN_CXX = ${SOURCE_DIRECTORY}/main.cxx
MAIN_OBJ = ${OBJECT_DIRECTORY}/main.o

release: DEFINES =
release: OPTIMIZATIONS = -O3
release: ${EXECUTABLE}

debug: DEFINES = -DDEBUG -g
debug: OPTIMIZATIONS = -Og
debug: ${EXECUTABLE}

dist: ${DISTRIBUTABLE}

ifeq (${STATIC_BUILD}, 0)
libglfw: bin/libglfw.so
bin/libglfw.so:
	cp $$(dpkg -L libglfw3-dev | grep libglfw.so) ${EXECUTABLE_DIRECTORY}/
else
libglfw:
endif

${DISTRIBUTABLE}: ${EXECUTABLE}
	zip -r ${DISTRIBUTABLE} ${EXECUTABLE} ${EXECUTABLE_DIRECTORY}/*.so ${SHADERS_DIRECTORY}

${EXECUTABLE}: ${EXECUTABLE_DIRECTORY} ${OBJECT_DIRECTORY} ${GRAPHICS_OBJ} ${IO_OBJ} ${MAIN_OBJ} libglfw
	${CXX} -o $@ ${OBJECT_DIRECTORY}/*.o ${LIBRARIES}

${EXECUTABLE_DIRECTORY}:
	mkdir -p ${EXECUTABLE_DIRECTORY}

${OBJECT_DIRECTORY}:
	mkdir -p ${OBJECT_DIRECTORY}

${GRAPHICS_OBJ}: ${GRAPHICS_CXX} ${GRAPHICS_HXX} ${DEBUG_HXX}
	${CXX} -c -o $@ $< ${WARNINGS} ${DEFINES} ${OPTIMIZATIONS} ${CXX_STANDARD} ${INCLUDES}

${IO_OBJ}: ${IO_CXX} ${IO_HXX} ${DEBUG_HXX}
	${CXX} -c -o $@ $< ${WARNINGS} ${DEFINES} ${OPTIMIZATIONS} ${CXX_STANDARD} ${INCLUDES}

${MAIN_OBJ}: ${MAIN_CXX} ${DEBUG_HXX}
	${CXX} -c -o $@ $< ${WARNINGS} ${DEFINES} ${OPTIMIZATIONS} ${CXX_STANDARD} ${INCLUDES}

clean:
	rm -rf ${EXECUTABLE_DIRECTORY} ${OBJECT_DIRECTORY}
