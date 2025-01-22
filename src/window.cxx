#include "window.hxx"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "debug.hxx"
#include "icon.hxx"

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
  window = glfwCreateWindow(400, 400, "ShaderTest", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error{"Failed to create GLFW window"};
  }
  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, [](
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
  ) -> void {
    const auto windowOwner{
      static_cast<WindowOwner*>(glfwGetWindowUserPointer(window))
    };
    windowOwner->onKey(window, key, scancode, action, mods);
  });

  glfwSetWindowUserPointer(window, this);
  const GLFWimage icon_data{mainIconWidth, mainIconHeight, static_cast<unsigned char*>(mainIcon)};
  glfwSetWindowIcon(window, 1, &icon_data);
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

auto WindowOwner::onKey(
  GLFWwindow* window, int key, int /*scancode*/, int action, int mods
) -> void {
  const int keyUp{action & GLFW_KEY_UP};
  const bool keyQ{key == GLFW_KEY_Q};
  const bool keyW{key == GLFW_KEY_W};
  const bool keyF4{key == GLFW_KEY_F4};
  const int keyCtrl{mods & GLFW_MOD_CONTROL};
  const int keyAlt{mods & GLFW_MOD_ALT};
  if (keyUp && ((keyCtrl && (keyQ || keyW)) || (keyAlt && keyF4))) {
    glfwSetWindowShouldClose(window, true);
    return;
  }
}
