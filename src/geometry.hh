#ifndef GEOMETRY_HH
#define GEOMETRY_HH

#include <array> // array

namespace graphics {
auto vertices{std::array<float, 8>{
  -1.f, -1.f, -1.f, 1.f, 1.f, 1.f, 1.f, -1.f,
}};

auto indices{std::array<unsigned int, 6>{
  // Left triangle
  0u,
  1u,
  2u,
  // Right triangle
  0u,
  2u,
  3u,
}};
} // namespace graphics

#endif // GEOMETRY_HH
