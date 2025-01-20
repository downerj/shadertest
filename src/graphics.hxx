#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "debug.hxx"

constexpr const char* defaultVertexSource{
#include "default.vert"
};

constexpr const char* defaultFragmentSource{
#include "default.frag"
};

#ifdef DEBUG
auto debugMessageCallbackGL(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam
) -> void;

auto errorCallbackGLFW(int error, const char* description) -> void;
#endif // DEBUG

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

auto createShader(GLenum type, std::string_view source) -> GLuint;
auto createProgram(
  std::string_view vertexSource,
  std::string_view fragmentSource
) -> std::optional<GLuint>;
auto initializeWindow() -> GLFWwindow*;
auto generateShaderData(
  std::string_view vertexSource,
  std::string_view fragmentSource
) -> std::optional<ShaderData>;
auto render(GLFWwindow* window, const ShaderData& shaderData) -> void;
auto cleanup(GLFWwindow* window, ShaderData& shaderData) -> void;

#endif // GRAPHICS_HXX
