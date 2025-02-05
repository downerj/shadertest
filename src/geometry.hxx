#ifndef MODELS_HXX
#define MODELS_HXX

#include <array>
#include <memory>

enum class GeometryType {
  Rectangle,
  Triangle
};

class Geometry {
public:
  static auto createGeometryFromType(
    GeometryType type
  ) -> std::unique_ptr<Geometry>;
  virtual auto getVertices() const -> const float* = 0;
  virtual auto getIndices() const -> const unsigned short* = 0;
  virtual auto getVertexCount() const -> int = 0;
  virtual auto getIndexCount() const -> int = 0;
  virtual ~Geometry();
};

class Triangle : public Geometry {
public:
  auto getVertices() const -> const float* final;
  auto getIndices() const -> const unsigned short* final;
  auto getVertexCount() const -> int final;
  auto getIndexCount() const -> int final;

private:
  static const std::array<float, 3*3> _vertices;
  static const std::array<unsigned short, 1*3> _indices;
};

class Rectangle : public Geometry {
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
