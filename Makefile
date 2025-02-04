.PHONY: clean

# Environment variables.

STATIC_BUILD ?= 0
DEBUG_BUILD ?= 0

# Zip file.

ZIP_SUFFIX =
ifeq (${OS}, Windows_NT)
	# TODO: Target Win32 (e.g. MinGW, Cygwin)
	ZIP_SUFFIX += Win32
else
	UNAME_S = $(shell uname --kernel-name)
	ifeq (${UNAME_S}, Linux)
		LINUX_ID = $(shell lsb_release --id --short)
		LINUX_RELEASE = $(shell lsb_release --release --short)
		ZIP_SUFFIX += ${LINUX_ID}_${LINUX_RELEASE}
	endif
	ifeq (${UNAME_S}, Darwin)
		# TODO: Target macOS
		ZIP_SUFFIX += macOS
	endif
endif
ZIP_NAME = ShaderTest_${ZIP_SUFFIX}.zip
ZIP = ${EXE_DIR}/${ZIP_NAME}

# Executable and object directories.

EXE_DIR_PREFIX = bin
OBJ_DIR_PREFIX = obj
EXE_DIR =
OBJ_DIR =
ifeq (${DEBUG_BUILD}, 1)
	EXE_DIR += ${EXE_DIR_PREFIX}/debug
	OBJ_DIR += ${OBJ_DIR_PREFIX}/debug
else
	EXE_DIR += ${EXE_DIR_PREFIX}/release
	OBJ_DIR += ${OBJ_DIR_PREFIX}/release
endif
EXE_NAME = shadertest
EXE = ${EXE_DIR}/${EXE_NAME}

# Other directories.

SRC_DIR = src
LIB_DIR = lib
INCL_DIR = include
EXAMPLES_DIR = examples

# Source dependencies & object targets.

DEPS = $(wildcard ${OBJ_DIR}/*.d)
HEADERS = $(wildcard ${SRC_DIR}/*.hxx)
SOURCES = $(wildcard ${SRC_DIR}/*.cxx)
TARGETS = $(patsubst ${SRC_DIR}/%.cxx, ${OBJ_DIR}/%.o, ${SOURCES})

# Compiler flags.

WARNINGS = -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wunreachable-code
CXX_STANDARD = -std=c++17
DEFINES =
OPTIMIZATIONS =
ifeq (${DEBUG_BUILD}, 1)
	DEFINES += -DDEBUG -g
	OPTIMIZATIONS += -Og
else
	OPTIMIZATIONS += -O3
endif

# Libraries.

GLFW_LIBRARY =
ifeq (${STATIC_BUILD}, 1)
	GLFW_LIBRARY += -L"${LIB_DIR}" -l:libglfw3.a
else
	GLFW_LIBRARY += -lglfw
endif
INCLUDES = -I${INCL_DIR} ${GLFW_INCLUDE}
LIBRARIES = ${GLFW_LIBRARY}

# Recipes.

all: prebuild dist

prebuild:
	mkdir -p ${EXE_DIR}
	mkdir -p ${OBJ_DIR}

dist: ${ZIP}

${ZIP}: exe
	zip -r ${ZIP} ${EXE} ${EXAMPLES_DIR}

exe: ${EXE}

${EXE}: ${TARGETS}
	${CXX} -o $@ $^ ${LIBRARIES}

ifneq (${DEPS},)
include ${DEPS}
endif

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cxx
	${CXX} -MMD -c -o $@ $< ${WARNINGS} ${DEFINES} ${OPTIMIZATIONS} ${CXX_STANDARD} ${INCLUDES}

clean:
	${RM} -v ${EXE_DIR}/* ${OBJ_DIR}/*
