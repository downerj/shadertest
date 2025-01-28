#include "window.hxx"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "debug.hxx"
#include "icon.hxx"

#ifdef DEBUG
auto errorCallbackGLFW(
  int /*error*/, const char* description
) -> void {
  LOG_ERROR("GLFW error: " << description << '\n');
}
#endif

auto WindowActions::reset() -> void {
  changeModelType = false;
}

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
  window = glfwCreateWindow(
    initialWidth, initialHeight, title, nullptr, nullptr
  );
  if (!window) {
    glfwTerminate();
    throw std::runtime_error{"Failed to create GLFW window"};
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, WindowOwner::onKeyGLFW);
  glfwSetWindowUserPointer(window, this);
  const GLFWimage icon_data{
    mainIconWidth, mainIconHeight, static_cast<unsigned char*>(mainIcon)
  };
  glfwSetWindowIcon(window, 1, &icon_data);
}

WindowOwner::~WindowOwner() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

auto WindowOwner::getWindow() -> GLFWwindow* {
  return window;
}

auto WindowOwner::getActions() -> WindowActions& {
  return actions;
}

auto WindowOwner::isActive() -> bool {
  return !glfwWindowShouldClose(window);
}

auto WindowOwner::update() -> void {
  glfwSwapBuffers(window);
  glfwPollEvents();
}

auto WindowOwner::onKeyGLFW(
  GLFWwindow* window, int key, int scancode, int action, int mods
) -> void {
  const auto windowOwner{
    static_cast<WindowOwner*>(glfwGetWindowUserPointer(window))
  };
  windowOwner->onKey(window, key, scancode, action, mods);
}

auto WindowOwner::onKey(
  GLFWwindow* window, int key, int /*scancode*/, int action, int mods
) -> void {
  const bool closeKey1{
    action == GLFW_RELEASE && mods == GLFW_MOD_CONTROL && key == GLFW_KEY_Q
  };
  const bool closeKey2{
    action == GLFW_RELEASE && mods == GLFW_MOD_CONTROL && key == GLFW_KEY_W
  };
  const bool closeKey3{
    action == GLFW_RELEASE && mods == GLFW_MOD_ALT && key == GLFW_KEY_F4
  };
  const bool resetWindowKey{
    action == GLFW_RELEASE && mods == GLFW_MOD_CONTROL && key == GLFW_KEY_R
  };
  const bool model1Key{
    action == GLFW_RELEASE && mods == GLFW_MOD_ALT && key == GLFW_KEY_1
  };
  const bool model2Key{
    action == GLFW_RELEASE && mods == GLFW_MOD_ALT && key == GLFW_KEY_2
  };

  if (closeKey1 || closeKey2 || closeKey3) {
    glfwSetWindowShouldClose(window, true);
  } else if (resetWindowKey) {
    glfwSetWindowSize(window, initialWidth, initialHeight);
  } else if (model1Key) {
    actions.changeModelType = true;
    actions.modelType = ModelType::Rectangle;
  } else if (model2Key) {
    actions.changeModelType = true;
    actions.modelType = ModelType::Triangle;
  }
}
