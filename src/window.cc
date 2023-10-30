#include <stdexcept> // invalid_argument, logic_error

#include "window.hh"
#include "compatibility.hh"
#include "configurations.hh"

void onKey(GLFWwindow* window, int key, int /* scancode */, int action, int mods) {
  if ((key == GLFW_KEY_Q or key == GLFW_KEY_W) and mods & GLFW_MOD_CONTROL and action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

namespace graphics {
  GLFWwindow* createWindow(Configurations& configs) {
    if (configs.versionRequest == VersionRequest::Specific) {
      if (configs.profileRequest == ProfileRequest::Core || configs.profileRequest == ProfileRequest::Compat) {
        if (configs.wantVersionMajor < 3 or (configs.wantVersionMajor == 3 and configs.wantVersionMinor < 3)) {
          throw std::invalid_argument("Compatability profiles are only valid for GL>=3.3");
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
      configs.windowTitle,
      nullptr,
      nullptr
    );
  }

  void initializeWindow(Configurations& configs) {
    if (not glfwInit()) {
      throw std::logic_error("Cannot initialize GLFW");
    }

    if (configs.versionRequest != VersionRequest::Maximum) {
      configs.window = createWindow(configs);
    } else {
      for (unsigned int v = 0; v < possibleGLVersions.size(); v += 2) {
        configs.wantVersionMajor = possibleGLVersions[v];
        configs.wantVersionMinor = possibleGLVersions[v + 1];
        configs.window = createWindow(configs);
        if (configs.window) {
          break;
        }
      }
    }

    if (not configs.window) {
      throw std::logic_error("Cannot create window");
    }
    glfwMakeContextCurrent(configs.window);
    glfwSetKeyCallback(configs.window, onKey);
  }
}
