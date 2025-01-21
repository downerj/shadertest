#include "window.hxx"

#include <exception>

#include "debug.hxx"

#ifdef DEBUG
auto errorCallbackGLFW(
  int /*error*/,
  const char* description
) -> void {
  LOG_ERROR("GLFW error: " << description << '\n');
}
#endif

WindowOwner::WindowOwner() {
  if (!glfwInit()) {
    throw std::runtime_error{"Failed to initialize GLFW"};
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
  window = glfwCreateWindow(640, 480, "Hello", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error{"Failed to create GLFW window"};
  }
  glfwMakeContextCurrent(window);
}

WindowOwner::~WindowOwner() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

auto WindowOwner::getWindow() -> GLFWwindow* {
  return window;
}

auto WindowOwner::isActive() -> bool {
  return !glfwWindowShouldClose(window);
}
