#include <stdexcept> // invalid_argument, logic_error

#include "compatibility.hh"
#include "configurations.hh"
#include "window.hh"

using namespace std;

namespace graphics {
  WindowHandler::WindowHandler(Configurations& configs) : configs(configs) {
    if (not glfwInit()) {
      throw logic_error{"Cannot initialize GLFW"};
    }

    if (configs.versionRequest != VersionRequest::Maximum) {
      createWindow(configs);
    } else {
      for (auto v = 0u; v < possibleGLVersions.size(); v += 2u) {
        configs.wantVersionMajor = possibleGLVersions[v];
        configs.wantVersionMinor = possibleGLVersions[v + 1u];
        createWindow(configs);
        if (window) {
          break;
        }
      }
    }

    if (not window) {
      throw logic_error{"Cannot create window"};
    }
    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    // Note: A lambda can be used here as a callback only because it has no capturing group.
    // This allows the compiler to convert it to a function pointer.
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int /* scancode */, int action, int mods) {
      static_cast<WindowHandler*>(glfwGetWindowUserPointer(window))->onKey(key, action, mods);
    });
  }

  auto WindowHandler::createWindow(const Configurations& configs) -> void {
    if (configs.versionRequest == VersionRequest::Specific) {
      if (configs.profileRequest == ProfileRequest::Core || configs.profileRequest == ProfileRequest::Compat) {
        if (configs.wantVersionMajor < 3u or (configs.wantVersionMajor == 3u and configs.wantVersionMinor < 3u)) {
          throw invalid_argument{"Compatability profiles are only valid for GL>=3.3"};
        }
      }
    }

    if (configs.versionRequest != VersionRequest::Default) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, configs.wantVersionMajor);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, configs.wantVersionMinor);
    }
    if (configs.profileRequest == ProfileRequest::Core) {
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    } else if (configs.profileRequest == ProfileRequest::Compat) {
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    }

    window = glfwCreateWindow(
      configs.windowWidth,
      configs.windowHeight,
      configs.windowTitle.c_str(),
      nullptr,
      nullptr
    );
  }

  auto WindowHandler::onKey(int key, int action, int mods) -> void {
    const auto isCtrlQ = key == GLFW_KEY_Q and mods == GLFW_MOD_CONTROL;
    const auto isCtrlR = key == GLFW_KEY_R and mods == GLFW_MOD_CONTROL;
    const auto isCtrlW = key == GLFW_KEY_W and mods == GLFW_MOD_CONTROL;
    const auto isAltF4 = key == GLFW_KEY_F4 and mods == GLFW_MOD_ALT;
    const auto isPressed = action == GLFW_PRESS;
    if ((isCtrlQ or isCtrlW or isAltF4) and isPressed) {
      glfwSetWindowShouldClose(window, GL_TRUE);
    } else if (isCtrlR and action == GLFW_PRESS) {
      glfwSetWindowSize(window, configs.windowWidth, configs.windowHeight);
    }
  }
}
