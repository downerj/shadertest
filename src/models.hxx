#ifndef MODELS_HXX
#define MODELS_HXX

#include <vector>

class Model {
public:
  virtual auto getVertices() const -> const std::vector<float>& = 0;
  virtual auto getIndices() const -> const std::vector<unsigned short>& = 0;
};

class Triangle : public Model {
public:
  auto getVertices() const -> const std::vector<float>& final;
  auto getIndices() const -> const std::vector<unsigned short>& final;

private:
  const static std::vector<float> vertices;
  const static std::vector<unsigned short> indices;
};

class Rectangle : public Model {
public:
  auto getVertices() const -> const std::vector<float>& final;
  auto getIndices() const -> const std::vector<unsigned short>& final;

private:
  const static std::vector<float> vertices;
  const static std::vector<unsigned short> indices;
};

#endif // MODELS_HXX
