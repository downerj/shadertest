#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <optional>
#include <string>
#include <string_view>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE

#include "geometry.hxx"
#include "parameters.hxx"

struct GLFWwindow;

struct ShaderData {
  GLuint program;
  GLuint vao;
  GLsizei indexCount;
  GLint timeLocation;
  GLint resolutionLocation;

  ShaderData() = delete;
  ShaderData(
    GLuint program, GLuint vao, GLsizei indexCount, GLint timeLocation,
    GLint resolutionLocation
  );
};

class GraphicsEngine {
public:
  GraphicsEngine(
    GLFWwindow* window, const std::optional<ShaderSources>& sources,
    GeometryType modelType
  );
  GraphicsEngine() = delete;
  GraphicsEngine(const GraphicsEngine&) = delete;
  GraphicsEngine(GraphicsEngine&&) = delete;
  GraphicsEngine operator=(const GraphicsEngine&) = delete;
  GraphicsEngine operator=(GraphicsEngine&&) = delete;
  ~GraphicsEngine();

  static auto initializeGL() -> bool;
  auto resetWith(
    const std::optional<ShaderSources>& shaderSources,
    const std::optional<GeometryType>& modelType
  ) -> void;
  auto hasValidData() -> bool;
  auto render() -> void;

private:
  static auto createShader(GLenum type, std::string_view source) -> GLuint;
  static auto createProgram(
    const ShaderSources& sources
  ) -> std::optional<GLuint>;
  static auto createVertexArrayForModel(
    GLuint program, const Geometry* model
  ) -> GLuint;

  auto resetTime() -> void;

  GLFWwindow* _window;
  std::optional<ShaderData> _shaderData{};
  GLfloat _initialTime{};
  std::unique_ptr<Geometry> _model{};
};

#endif // GRAPHICS_HXX
