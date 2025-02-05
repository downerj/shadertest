#include "geometry.hxx"

auto Geometry::createGeometryFromType(
  GeometryType type
) -> std::unique_ptr<Geometry> {
  if (type == GeometryType::Rectangle) {
    return std::make_unique<Rectangle>();
  } else /* if (type == GeometryType::Triangle) */ {
    return std::make_unique<Triangle>();
  }
}

Geometry::~Geometry() {}

auto Triangle::getVertices() const -> const float* {
  return _vertices.data();
}

auto Triangle::getIndices() const -> const unsigned short* {
  return _indices.data();
}

auto Triangle::getVertexCount() const -> int {
  return _vertices.size();
}

auto Triangle::getIndexCount() const -> int {
  return _indices.size();
}

const std::array<float, 3*3> Triangle::_vertices{
  -1., -1., 0.,
  1., -1., 0.,
  0., 1., 0.
};

const std::array<unsigned short, 1*3> Triangle::_indices{
  0, 1, 2
};

auto Rectangle::getVertices() const -> const float* {
  return _vertices.data();
}

auto Rectangle::getIndices() const -> const unsigned short* {
  return _indices.data();
}

auto Rectangle::getVertexCount() const -> int {
  return _vertices.size();
}

auto Rectangle::getIndexCount() const -> int {
  return _indices.size();
}

const std::array<float, 4*3> Rectangle::_vertices{
  -1., -1., 0.,
  1., -1., 0.,
  1., 1., 0.,
  -1., 1., 0.
};

const std::array<unsigned short, 2*3> Rectangle::_indices{
  0, 1, 2,
  0, 2, 3
};
