#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <optional>
#include <string_view>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE

struct GLFWwindow;

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
  ) :
    program{program},
    vao{vao},
    indexCount{indexCount},
    timeLocation{timeLocation},
    resolutionLocation{resolutionLocation} {}
};

class GraphicsEngine {
public:
  GraphicsEngine(GLFWwindow* window);
  GraphicsEngine(
    GLFWwindow* window,
    std::string_view vertexSource,
    std::string_view fragmentSource
  );
  GraphicsEngine() = delete;
  GraphicsEngine(const GraphicsEngine&) = delete;
  GraphicsEngine(GraphicsEngine&&) = delete;
  GraphicsEngine operator=(const GraphicsEngine&) = delete;
  GraphicsEngine operator=(GraphicsEngine&&) = delete;
  ~GraphicsEngine();

  static auto initializeGL() -> bool;
  auto render() -> void;

private:
  static auto createShader(GLenum type, std::string_view source) -> GLuint;
  static auto createProgram(
    std::string_view vertexSource,
    std::string_view fragmentSource
  ) -> std::optional<GLuint>;
  static auto generateShaderData(
    std::string_view vertexSource,
    std::string_view fragmentSource
  ) -> std::optional<ShaderData>;

  GLFWwindow* window;
  std::optional<ShaderData> shaderData;
};

#endif // GRAPHICS_HXX
