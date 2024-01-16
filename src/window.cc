#include "window.hh"

#include <stdexcept> // invalid_argument, logic_error

#include "compatibility.hh"
#include "configurations.hh"

using namespace std;

namespace graphics {
WindowHandler::WindowHandler(Configurations& configs) : configs{configs} {
  if (not glfwInit()) {
    throw logic_error{"Cannot initialize GLFW"};
  }
  createWindow(configs);

  if (not window) {
    throw logic_error{"Cannot create window"};
  }
  glfwSetWindowUserPointer(window, this);
  glfwMakeContextCurrent(window);
  auto& [initialX, initialY] = initialPosition;
  glfwGetWindowPos(window, &initialX, &initialY);
  position = initialPosition;
  size = initialSize;
  // Note: A lambda can be used here as a callback only because it has no
  // capturing group. This allows the compiler to convert it to a function
  // pointer.
  glfwSetKeyCallback(
    window,
    [](GLFWwindow* window, int key, int /* scancode */, int action, int mods) {
      const auto handler{static_cast<WindowHandler*>(glfwGetWindowUserPointer(window))};
      handler->onKey(key, action, mods);
    }
  );
  glfwSetWindowPosCallback(window, [](GLFWwindow* window, int x, int y) {
    const auto handler{static_cast<WindowHandler*>(glfwGetWindowUserPointer(window))};
    handler->onMove(x, y);
  });
  glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    const auto handler{static_cast<WindowHandler*>(glfwGetWindowUserPointer(window))};
    handler->onResize(width, height);
  });
}

auto WindowHandler::createWindow(const Configurations& configs) -> void {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  const auto& [initialWidth, initialHeight] = initialSize;
  window
    = glfwCreateWindow(initialWidth, initialHeight, configs.windowTitle.c_str(), nullptr, nullptr);
}

auto WindowHandler::onKey(int key, int action, int mods) -> void {
  const auto isCtrlP = key == GLFW_KEY_P and mods == GLFW_MOD_CONTROL;
  const auto isCtrlQ = key == GLFW_KEY_Q and mods == GLFW_MOD_CONTROL;
  const auto isCtrlR = key == GLFW_KEY_R and mods == GLFW_MOD_CONTROL;
  const auto isCtrlW = key == GLFW_KEY_W and mods == GLFW_MOD_CONTROL;
  const auto isAltF4 = key == GLFW_KEY_F4 and mods == GLFW_MOD_ALT;
  const auto isF11 = key == GLFW_KEY_F11 and mods == 0;
  const auto isPressed = action == GLFW_PRESS;
  // Close window.
  if ((isCtrlQ or isCtrlW or isAltF4) and isPressed) {
    glfwSetWindowShouldClose(window, GL_TRUE);
    // Reset window position & size.
  } else if (isCtrlR and action == GLFW_PRESS) {
    if (!isFullScreen) {
      const auto& [x, y] = initialPosition;
      const auto& [width, height] = initialSize;
      glfwSetWindowPos(window, x, y);
      glfwSetWindowSize(window, width, height);
    }
    // Toggle fullscreen.
  } else if (isF11 and action == GLFW_PRESS) {
    isFullScreen = !isFullScreen;
    auto primaryMonitor = glfwGetPrimaryMonitor();
    if (isFullScreen) {
      glfwGetWindowPos(window, &position.x, &position.y);
      const auto monitorMode = glfwGetVideoMode(primaryMonitor);
      glfwSetWindowMonitor(
        window, primaryMonitor, 0, 0, monitorMode->width, monitorMode->height,
        monitorMode->refreshRate
      );
    } else {
      glfwSetWindowMonitor(window, nullptr, position.x, position.y, size.width, size.height, 0);
    }
    // Toggle pause animation.
  } else if (isCtrlP and action == GLFW_PRESS) {
    isAnimationPaused = !isAnimationPaused;
  }
}

auto WindowHandler::onMove(int x, int y) -> void {
  if (isFullScreen) {
    return;
  }
  position.x = x;
  position.y = y;
}

auto WindowHandler::onResize(int width, int height) -> void {
  if (isFullScreen) {
    return;
  }
  size.width = width;
  size.height = height;
}
} // namespace graphics
