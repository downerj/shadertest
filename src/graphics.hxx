#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <optional>
#include <string>
#include <string_view>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "debug.hxx"

constexpr const char* defaultVertexSource{R"(#version 110
#define positionOut gl_Position
#if __VERSION__ <= 130
attribute vec4 position;
#else
in vec4 position;
#endif

void main(void) {
  positionOut = position;
}
)"};

constexpr const char* defaultFragmentSource{R"(#version 110
#define fragCoordIn gl_FragCoord
#if __VERSION__ <= 130
#define fragColorOut gl_FragColor
#else
out vec4 fragColorOut;
#endif

void main(void) {
  fragColorOut = vec4(1., 0., 0., 1.);
}
)"};

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
  GLsizei vertexCount;

  ShaderData() = delete;
  ShaderData(GLuint program, GLuint vao, GLsizei vertexCount) :
    program{program},
    vao{vao},
    vertexCount{vertexCount} {}
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
