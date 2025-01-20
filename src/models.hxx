#ifndef MODELS_HXX
#define MODELS_HXX

#include <vector>

class Model {
public:
  Model(
    const std::vector<float>& vertices,
    const std::vector<unsigned short>& indices
  );
  Model() = delete;
  Model(const Model&) = delete;
  Model(Model&&) = delete;
  Model operator=(const Model&) = delete;
  Model operator=(Model&&) = delete;

  auto getVertices() const -> const std::vector<float>&;
  auto getIndices() const -> const std::vector<unsigned short>&;

private:
  const std::vector<float>& vertices;
  const std::vector<unsigned short>& indices;
};

class ModelSupplier {
public:
  ModelSupplier() = default;
  ModelSupplier(const ModelSupplier&) = delete;
  ModelSupplier(ModelSupplier&&) = delete;
  ModelSupplier operator=(const ModelSupplier&) = delete;
  ModelSupplier operator=(ModelSupplier&&) = delete;

  auto supplyTriangle() const -> const Model&;
  auto supplyRectangle() const -> const Model&;

private:
  const std::vector<float> triangleVertices{
    -1., -1., 0.,
    1., -1., 0.,
    0., 1., 0.
  };

  const std::vector<unsigned short> triangleIndices{
    0, 1, 2
  };

  const std::vector<float> rectangleVertices{
    -1., -1., 0.,
    1., -1., 0.,
    1., 1., 0.,
    -1., 1., 0.
  };

  const std::vector<unsigned short> rectangleIndices{
    0, 1, 2,
    0, 2, 3
  };

  const Model triangle{triangleVertices, triangleIndices};
  const Model rectangle{rectangleVertices, rectangleIndices};
};

#endif // MODELS_HXX
