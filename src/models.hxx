#ifndef MODELS_HXX
#define MODELS_HXX

#include <array>
#include <memory>

enum class ModelType {
  Rectangle,
  Triangle
};

class Model {
public:
  static auto createModelFromType(ModelType type) -> std::unique_ptr<Model>;
  virtual auto getVertices() const -> const float* = 0;
  virtual auto getIndices() const -> const unsigned short* = 0;
  virtual auto getVertexCount() const -> int = 0;
  virtual auto getIndexCount() const -> int = 0;
};

class Triangle : public Model {
public:
  auto getVertices() const -> const float* final;
  auto getIndices() const -> const unsigned short* final;
  auto getVertexCount() const -> int final;
  auto getIndexCount() const -> int final;

private:
  static const std::array<float, 3*3> _vertices;
  static const std::array<unsigned short, 1*3> _indices;
};

class Rectangle : public Model {
public:
  auto getVertices() const -> const float* final;
  auto getIndices() const -> const unsigned short* final;
  auto getVertexCount() const -> int final;
  auto getIndexCount() const -> int final;

private:
  static const std::array<float, 4*3> _vertices;
  static const std::array<unsigned short, 2*3> _indices;
};

#endif // MODELS_HXX
