#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <optional>
#include <string>
#include <string_view>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE

#include "models.hxx"
#include "window.hxx"

struct GLFWwindow;

struct ShaderSources {
  std::optional<std::string> vertexSource{};
  std::optional<std::string> fragmentSource{};
};

struct ShaderData {
  GLuint program;
  GLuint vao;
  GLsizei indexCount;
  GLint timeLocation;
  GLint resolutionLocation;

  ShaderData() = delete;
  ShaderData(
    GLuint program,
    GLuint vao,
    GLsizei indexCount,
    GLint timeLocation,
    GLint resolutionLocation
  );
};

class GraphicsEngine {
public:
  GraphicsEngine(
    GLFWwindow* window,
    const ShaderSources& sources,
    ModelType modelType
  );
  GraphicsEngine() = delete;
  GraphicsEngine(const GraphicsEngine&) = delete;
  GraphicsEngine(GraphicsEngine&&) = delete;
  GraphicsEngine operator=(const GraphicsEngine&) = delete;
  GraphicsEngine operator=(GraphicsEngine&&) = delete;
  ~GraphicsEngine();

  static auto initializeGL() -> bool;
  auto resetShaderData(
    const ShaderSources& sources,
    ModelType modelType
  ) -> void;
  auto render() -> void;

private:
  static auto createShader(GLenum type, std::string_view source) -> GLuint;
  static auto createProgram(
    std::string_view vertexSource,
    std::string_view fragmentSource
  ) -> std::optional<GLuint>;
  static auto generateShaderData(
    const ShaderSources& sources,
    ModelType modelType
  ) -> std::optional<ShaderData>;
  static auto cleanupShaderData(std::optional<ShaderData>& shaderData) -> void;

  GLFWwindow* window;
  std::optional<ShaderData> shaderData{};
  GLfloat initialTime{};
};

#endif // GRAPHICS_HXX
