#ifndef GL_DEFINITIONS_HH
#define GL_DEFINITIONS_HH

#include <vector>

#include <GL/glew.h>

namespace graphics {
  const std::vector<GLuint> possibleGLVersions = {
    4, 6,
    4, 5,
    4, 4,
    4, 3,
    4, 2,
    4, 1,
    4, 0,
    3, 3,
    3, 2,
    3, 1,
    3, 0,
    2, 1,
    2, 0,
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
}

#endif // GL_DEFINITIONS_HH
