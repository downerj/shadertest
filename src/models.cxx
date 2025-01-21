#include "models.hxx"

Model::Model(
  const std::vector<float>& vertices,
  const std::vector<unsigned short>& indices
) : vertices{vertices}, indices{indices} {}

auto Model::getVertices() const -> const std::vector<float>& {
  return vertices;
}

auto Model::getIndices() const -> const std::vector<unsigned short>& {
  return indices;
}
