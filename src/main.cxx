#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include "debug.hxx"
#include "graphics.hxx"
#include "io.hxx"

int main(int, char**) {
  if (!glfwInit()) {
    LOG_ERROR("Failed to initialize GLFW\n");
    std::exit(EXIT_FAILURE);
  }
#ifdef DEBUG
  glfwSetErrorCallback(errorCallbackGLFW);
#endif
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DECORATED, true);
#ifdef DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
  GLFWwindow* window{glfwCreateWindow(640, 480, "Hello", nullptr, nullptr)};
  if (window == nullptr) {
    LOG_ERROR("Failed to create GLFW window\n");
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
#ifdef DEBUG
  if (GLAD_GL_ARB_debug_output) {
    LOG("GL extension GL_ARB_debug_output available\n");
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(debugMessageCallbackGL, nullptr /*userParam*/);
  }
  else {
    LOG("GL extension GL_ARB_debug_output unavailable\n");
  }
#endif
  LOG("Running...\n");
  while (!glfwWindowShouldClose(window)) {
    int width{};
    int height{};
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0., .5, 1., 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  LOG("Goodbye.\n");
}
