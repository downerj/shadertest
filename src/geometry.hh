#ifndef GEOMETRY_HH
#define GEOMETRY_HH

namespace graphics {
  float vertices[] = {
    -1.0, -1.0,
    -1.0, 1.0,
    1.0, 1.0,
    1.0, -1.0,
  };

  unsigned int indices[] = {
    // Left triangle
    0, 1, 2,
    // Right triangle
    0, 2, 3,
  };
}

#endif // GEOMETRY_HH
