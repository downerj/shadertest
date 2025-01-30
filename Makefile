.PHONY: clean

DIST_SUFFIX =
ifeq (${OS}, Windows_NT)
	# TODO: Target Win32 (e.g. MinGW, Cygwin)
	DIST_SUFFIX += Win32
else
	UNAME_S = $(shell uname --kernel-name)
	ifeq (${UNAME_S}, Linux)
		LINUX_ID = $(shell lsb_release --id --short)
		LINUX_RELEASE = $(shell lsb_release --release --short)
		DIST_SUFFIX += ${LINUX_ID}_${LINUX_RELEASE}
	endif
	ifeq (${UNAME_S}, Darwin)
		# TODO: Target macOS
		DIST_SUFFIX += macOS
	endif
endif

EXECUTABLE_DIRECTORY = bin
OBJECT_DIRECTORY = obj
SOURCE_DIRECTORY = src
LIBRARY_DIRECTORY = lib
INCLUDE_DIRECTORY = include
SHADERS_DIRECTORY = examples
EXECUTABLE = ${EXECUTABLE_DIRECTORY}/shadertest
DISTRIBUTABLE = ${EXECUTABLE_DIRECTORY}/ShaderTest_${DIST_SUFFIX}.zip
WARNINGS = -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wunreachable-code
CXX_STANDARD = -std=c++17

STATIC_BUILD ?= 0

ifeq (${STATIC_BUILD}, 1)
GLFW_LIBRARY = -L"${LIBRARY_DIRECTORY}" -l:libglfw3.a
else
GLFW_LIBRARY = -lglfw
endif
INCLUDES = -I${INCLUDE_DIRECTORY} ${GLFW_INCLUDE}
LIBRARIES = ${GLFW_LIBRARY}
DEPS = $(wildcard ${OBJECT_DIRECTORY}/*.d)
HEADERS = $(wildcard ${SOURCE_DIRECTORY}/*.hxx)
SOURCES = $(wildcard ${SOURCE_DIRECTORY}/*.cxx)
TARGETS = $(patsubst ${SOURCE_DIRECTORY}/%.cxx, ${OBJECT_DIRECTORY}/%.o, ${SOURCES})

all: release dist

release: DEFINES =
release: OPTIMIZATIONS = -O3
release: ${EXECUTABLE}

debug: DEFINES = -DDEBUG -g
debug: OPTIMIZATIONS = -Og
debug: ${EXECUTABLE}

dist: ${DISTRIBUTABLE}

${DISTRIBUTABLE}: ${EXECUTABLE}
	zip -r ${DISTRIBUTABLE} ${EXECUTABLE} ${SHADERS_DIRECTORY}

ifeq (${STATIC_BUILD}, 0)
${EXECUTABLE}: ${EXECUTABLE_DIRECTORY} ${OBJECT_DIRECTORY} ${LIBGLFW}
else
${EXECUTABLE}: ${EXECUTABLE_DIRECTORY} ${OBJECT_DIRECTORY}
endif
${EXECUTABLE}: ${TARGETS}
	${CXX} -o $@ ${TARGETS} ${LIBRARIES}

${EXECUTABLE_DIRECTORY}:
	mkdir -p ${EXECUTABLE_DIRECTORY}

${OBJECT_DIRECTORY}:
	mkdir -p ${OBJECT_DIRECTORY}

ifneq (${DEPS},)
include ${DEPS}
endif

${OBJECT_DIRECTORY}/%.o: ${SOURCE_DIRECTORY}/%.cxx
	${CXX} -MMD -c -o $@ $< ${WARNINGS} ${DEFINES} ${OPTIMIZATIONS} ${CXX_STANDARD} ${INCLUDES}

clean:
	${RM} -v ${EXECUTABLE_DIRECTORY}/* ${OBJECT_DIRECTORY}/*
