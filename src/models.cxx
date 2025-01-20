#include "models.hxx"

const std::vector<float> Triangle::vertices{
  -1., -1., 0.,
  1., -1., 0.,
  0., 1., 0.
};

const std::vector<unsigned short> Triangle::indices{
  0, 1, 2
};

auto Triangle::getVertices() const -> const std::vector<float>& {
  return vertices;
}

auto Triangle::getIndices() const -> const std::vector<unsigned short>& {
  return indices;
}

const std::vector<float> Rectangle::vertices{
  -1., -1., 0.,
  1., -1., 0.,
  1., 1., 0.,
  -1., 1., 0.,
};

const std::vector<unsigned short> Rectangle::indices{
  0, 1, 2,
  0, 2, 3,
};

auto Rectangle::getVertices() const -> const std::vector<float>& {
  return vertices;
}

auto Rectangle::getIndices() const -> const std::vector<unsigned short>& {
  return indices;
}
