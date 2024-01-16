#ifndef CONFIGS_HH
#define CONFIGS_HH

#include <string> // string

#include "compatibility.hh"
#include "gl-includes.hh"

namespace graphics {
using namespace std::literals;
struct Configurations {
public:
  std::string fragmentFilePath;
  std::string vertexSource;
  std::string fragmentSource;
  bool wantHelpOnly{false};
  bool wantInfoOnly{false};
  std::string windowTitle{"Shader Test"s};
};
} // namespace graphics

#endif // CONFIGS_HH
