#include <stdexcept> // invalid_argument, logic_error

#include "window.hh"
#include "compatibility.hh"
#include "configurations.hh"

using namespace std;

auto onKey(GLFWwindow* window, int key, int /* scancode */, int action, int mods) -> void {
  if ((key == GLFW_KEY_Q or key == GLFW_KEY_W) and mods & GLFW_MOD_CONTROL and action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

namespace graphics {
  auto createWindow(Configurations& configs) -> GLFWwindow* {
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

    return glfwCreateWindow(
      configs.windowWidth,
      configs.windowHeight,
      configs.windowTitle.c_str(),
      nullptr,
      nullptr
    );
  }

  auto initializeWindow(Configurations& configs) -> void {
    if (not glfwInit()) {
      throw logic_error{"Cannot initialize GLFW"};
    }

    if (configs.versionRequest != VersionRequest::Maximum) {
      configs.window = createWindow(configs);
    } else {
      for (auto v = 0u; v < possibleGLVersions.size(); v += 2u) {
        configs.wantVersionMajor = possibleGLVersions[v];
        configs.wantVersionMinor = possibleGLVersions[v + 1u];
        configs.window = createWindow(configs);
        if (configs.window) {
          break;
        }
      }
    }

    if (not configs.window) {
      throw logic_error{"Cannot create window"};
    }
    glfwMakeContextCurrent(configs.window);
    glfwSetKeyCallback(configs.window, onKey);
  }
}
