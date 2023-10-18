#ifndef CONFIGS_HH
#define CONFIGS_HH

#include <string> // string

#include "gl-includes.hh"
#include "compatibility.hh"

namespace graphics {
  class Configurations {
  public:
    std::string fragmentFilePath;
    std::string vertexSource;
    std::string fragmentSource;
    bool wantInfoOnly = false;
    VersionRequest versionRequest = VersionRequest::Default;
    unsigned int wantVersionMajor = 0;
    unsigned int wantVersionMinor = 0;
    ProfileRequest profileRequest = ProfileRequest::Any;
    unsigned int windowWidth = 400;
    unsigned int windowHeight = 400;
    const char* windowTitle = "Shader Test";
    GLFWwindow* window = nullptr;
  };
}

#endif // CONFIGS_HH
