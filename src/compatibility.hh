#ifndef GL_DEFINITIONS_HH
#define GL_DEFINITIONS_HH

#include <vector> // vector

#include "gl-includes.hh"

namespace graphics {
const auto possibleGLVersions = std::vector<GLuint>{
  4u, 6u, 4u, 5u, 4u, 4u, 4u, 3u, 4u, 2u, 4u, 1u, 4u,
  0u, 3u, 3u, 3u, 2u, 3u, 1u, 3u, 0u, 2u, 1u, 2u, 0u,
};

enum class ProfileRequest {
  Any,
  Compat,
  Core,
};

enum class VersionRequest {
  Default,
  Specific,
  Maximum,
};
} // namespace graphics

#endif // GL_DEFINITIONS_HH
