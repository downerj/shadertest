#ifndef GL_DEFINITIONS_HH
#define GL_DEFINITIONS_HH

#include <array>

#include "gl-includes.hh"

namespace graphics {
const std::array<GLuint, 26> possibleGLVersions{
  4, 6, 4, 5, 4, 4, 4, 3, 4, 2, 4, 1, 4,
  0, 3, 3, 3, 2, 3, 1, 3, 0, 2, 1, 2, 0,
};
} // namespace graphics

#endif // GL_DEFINITIONS_HH
