#ifndef GEOMETRY_HH
#define GEOMETRY_HH

#include <array>

#include <glad/gl.h>

namespace graphics {
constexpr std::array<GLfloat, 8> vertices{
  -1., -1.,
  -1., 1.,
  1., 1.,
  1., -1.,
};

constexpr std::array<GLuint, 6> indices{
  // Left triangle
  0, 1, 2,
  // Right triangle
  0, 2, 3,
};
} // namespace graphics

#endif // GEOMETRY_HH
