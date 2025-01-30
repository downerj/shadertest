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
  closeWindow = false;
  resetWindowSize = false;
  pauseResume = false;
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
  _window = glfwCreateWindow(
    _initialWidth, _initialHeight, _title, nullptr, nullptr
  );
  if (!_window) {
    glfwTerminate();
    throw std::runtime_error{"Failed to create GLFW window"};
  }
  glfwMakeContextCurrent(_window);
  glfwSetKeyCallback(_window, WindowOwner::onKeyGLFW);
  glfwSetWindowUserPointer(_window, this);
  const GLFWimage icon_data{
    mainIconWidth, mainIconHeight, static_cast<unsigned char*>(mainIcon)
  };
  glfwSetWindowIcon(_window, 1, &icon_data);
}

WindowOwner::~WindowOwner() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

auto WindowOwner::getWindow() -> GLFWwindow* {
  return _window;
}

auto WindowOwner::getActions() -> WindowActions& {
  return _actions;
}

auto WindowOwner::isActive() -> bool {
  return !glfwWindowShouldClose(_window);
}

auto WindowOwner::closeWindow() -> void {
  glfwSetWindowShouldClose(_window, true);
}

auto WindowOwner::resetWindowSize() -> void {
  glfwSetWindowSize(_window, _initialWidth, _initialHeight);
}

auto WindowOwner::update() -> void {
  glfwSwapBuffers(_window);
  glfwPollEvents();
}

auto WindowOwner::onKeyGLFW(
  GLFWwindow* window, int key, int /*scancode*/, int action, int mods
) -> void {
  const auto windowOwner{
    static_cast<WindowOwner*>(glfwGetWindowUserPointer(window))
  };
  if (windowOwner) {
    windowOwner->onKey(key, action, mods);
  }
}

auto WindowOwner::onKey(
  int key, int action, int mods
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
  const bool pauseResumeKey{
    action == GLFW_RELEASE && mods == 0 && key == GLFW_KEY_SPACE
  };

  if (closeKey1 || closeKey2 || closeKey3) {
    _actions.closeWindow = true;
  } else if (resetWindowKey) {
    _actions.resetWindowSize = true;
  } else if (model1Key) {
    _actions.changeModelType = true;
    _actions.modelType = GeometryType::Rectangle;
  } else if (model2Key) {
    _actions.changeModelType = true;
    _actions.modelType = GeometryType::Triangle;
  } else if (pauseResumeKey) {
    _actions.pauseResume = true;
  }
}
