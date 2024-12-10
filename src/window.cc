#include "window.hh"

#include <stdexcept>

#include "compatibility.hh"
#include "configurations.hh"

namespace graphics {
WindowHandler::WindowHandler(Configurations& configs) : configs{configs} {
  if (not glfwInit()) {
    throw std::logic_error{"Cannot initialize GLFW"};
  }
  createWindow(configs);

  if (not window) {
    throw std::logic_error{"Cannot create window"};
  }
  glfwSetWindowUserPointer(window, this);
  glfwMakeContextCurrent(window);
  auto& [initialX, initialY]{initialPosition};
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

void WindowHandler::createWindow(const Configurations& configs) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  const auto& [initialWidth, initialHeight]{initialSize};
  window
    = glfwCreateWindow(initialWidth, initialHeight, configs.windowTitle.c_str(), nullptr, nullptr);
}

void WindowHandler::onKey(int key, int action, int mods) {
  const bool isCtrlP{key == GLFW_KEY_P and mods == GLFW_MOD_CONTROL};
  const bool isCtrlQ{key == GLFW_KEY_Q and mods == GLFW_MOD_CONTROL};
  const bool isCtrlR{key == GLFW_KEY_R and mods == GLFW_MOD_CONTROL};
  const bool isCtrlW{key == GLFW_KEY_W and mods == GLFW_MOD_CONTROL};
  const bool isAltF4{key == GLFW_KEY_F4 and mods == GLFW_MOD_ALT};
  const bool isF11{key == GLFW_KEY_F11 and mods == 0};
  const bool isPressed{action == GLFW_PRESS};
  // Close window.
  if ((isCtrlQ or isCtrlW or isAltF4) and isPressed) {
    glfwSetWindowShouldClose(window, GL_TRUE);
    // Reset window position & size.
  } else if (isCtrlR and action == GLFW_PRESS) {
    if (!isFullScreen) {
      const auto& [x, y]{initialPosition};
      const auto& [width, height]{initialSize};
      glfwSetWindowPos(window, x, y);
      glfwSetWindowSize(window, width, height);
    }
    // Toggle fullscreen.
  } else if (isF11 and action == GLFW_PRESS) {
    isFullScreen = !isFullScreen;
    GLFWmonitor* primaryMonitor{glfwGetPrimaryMonitor()};
    if (isFullScreen) {
      glfwGetWindowPos(window, &position.x, &position.y);
      const GLFWvidmode* monitorMode{glfwGetVideoMode(primaryMonitor)};
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

void WindowHandler::onMove(int x, int y) {
  if (isFullScreen) {
    return;
  }
  position.x = x;
  position.y = y;
}

void WindowHandler::onResize(int width, int height) {
  if (isFullScreen) {
    return;
  }
  size.width = width;
  size.height = height;
}
} // namespace graphics
