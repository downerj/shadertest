#include "models.hxx"

auto Triangle::getVertices() const -> const float* {
  return vertices.data();
}

auto Triangle::getIndices() const -> const unsigned short* {
  return indices.data();
}

auto Triangle::getVertexCount() const -> int {
  return vertices.size();
}

auto Triangle::getIndexCount() const -> int {
  return indices.size();
}

const std::array<float, 3*3> Triangle::vertices{
  -1., -1., 0.,
  1., -1., 0.,
  0., 1., 0.
};

const std::array<unsigned short, 1*3> Triangle::indices{
  0, 1, 2
};

auto Rectangle::getVertices() const -> const float* {
  return vertices.data();
}

auto Rectangle::getIndices() const -> const unsigned short* {
  return indices.data();
}

auto Rectangle::getVertexCount() const -> int {
  return vertices.size();
}

auto Rectangle::getIndexCount() const -> int {
  return indices.size();
}

const std::array<float, 4*3> Rectangle::vertices{
  -1., -1., 0.,
  1., -1., 0.,
  1., 1., 0.,
  -1., 1., 0.
};

const std::array<unsigned short, 2*3> Rectangle::indices{
  0, 1, 2,
  0, 2, 3
};
