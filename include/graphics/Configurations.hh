#ifndef CONFIGS_HH
#define CONFIGS_HH

#include <string>

#include <GLFW/glfw3.h>

#include <graphics/Compatibility.hh>

namespace graphics {
  class Configurations {
  public:
    std::string fragmentFilePath;
    std::string vertexSource;
    std::string fragmentSource;
    bool wantInfoOnly = false;
    graphics::VersionRequest versionRequest = graphics::VersionRequest::Default;
    unsigned int wantVersionMajor = 0;
    unsigned int wantVersionMinor = 0;
    graphics::ProfileRequest profileRequest = graphics::ProfileRequest::Any;
    unsigned int windowWidth = 400;
    unsigned int windowHeight = 400;
    const char* windowTitle = "Shader Test";
    GLFWwindow* window = nullptr;
  };
}

#endif // CONFIGS_HH
