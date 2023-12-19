#ifndef CONFIGS_HH
#define CONFIGS_HH

#include <string> // string

#include "gl-includes.hh"
#include "compatibility.hh"

namespace graphics {
  using namespace std::literals;
  struct Configurations {
  public:
    std::string fragmentFilePath;
    std::string vertexSource;
    std::string fragmentSource;
    bool wantInfoOnly = false;
    VersionRequest versionRequest = VersionRequest::Default;
    unsigned int wantVersionMajor = 0u;
    unsigned int wantVersionMinor = 0u;
    ProfileRequest profileRequest = ProfileRequest::Any;
    unsigned int windowWidth = 400u;
    unsigned int windowHeight = 400u;
    std::string windowTitle = "Shader Test"s;
    GLFWwindow* window = nullptr;
  };
}

#endif // CONFIGS_HH
