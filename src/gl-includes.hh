#ifndef GL_INCLUDES_HH
#define GL_INCLUDES_HH

// Ensure that GLEW (and therefore OpenGL) is included before GLFW.
#include <GL/glew.h>
#ifdef _WIN32
//#define GLFW_DLL
#endif // _WIN32
#include <GLFW/glfw3.h>

#endif // GL_INCLUDES_HH
