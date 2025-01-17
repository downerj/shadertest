#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <array>
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
attribute vec3 position;
#else
in vec3 position;
#endif

uniform float time;
uniform ivec2 resolution;

void main(void) {
  positionOut = vec4(position, 1.);
}
)"};

constexpr const char* defaultFragmentSource{R"(#version 110
#define fragCoordIn gl_FragCoord
#if __VERSION__ <= 130
#define fragColorOut gl_FragColor
#else
out vec4 fragColorOut;
#endif

uniform float time;
uniform ivec2 resolution;

vec4 hsv2rgba(in vec3 hsv) {
  float h = hsv.x;
  float s = hsv.y;
  float v = hsv.z;
  vec3 k = vec3(1., 2./3., 1./3.);
  vec3 p = clamp(abs(6.*fract(h - k) - 3.) - 1., 0., 1.);
  return vec4(v * mix(k.xxx, p, s), 1.);
}

void main(void) {
  vec2 center = vec2(resolution)*.5;
  float scale = min(float(resolution.x), float(resolution.y));
  vec2 newCenter = (fragCoordIn.xy - center)/scale;
  vec3 hsv = vec3(newCenter.y - newCenter.x + time/10., 1., 1.);
  fragColorOut = hsv2rgba(hsv);
}
)"};

template<GLsizei V, GLsizei I>
class Model {
public:
  virtual auto getVertices() const -> const std::array<GLfloat, V>& = 0;
  virtual auto getIndices() const -> const std::array<GLshort, I>& = 0;
  auto getVertexCount() const -> GLsizei { return V; }
  auto getIndexCount() const -> GLsizei { return I; }
};

class Rectangle : public Model<12, 6> {
public:
  auto getVertices() const -> const std::array<GLfloat, 12>& final;
  auto getIndices() const -> const std::array<GLshort, 6>& final;

private:
  const static std::array<GLfloat, 12> vertices;
  const static std::array<GLshort, 6> indices;
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
