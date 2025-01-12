#ifndef GRAPHICS_HXX
#define GRAPHICS_HXX

#include <optional>
#include <string>
#include <string_view>

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "debug.hxx"

#ifdef DEBUG
void debugMessageCallbackGL(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam
);

void errorCallbackGLFW(int error, const char* description);
#endif // DEBUG

struct ProgramData {
  GLuint program;
  GLuint vao;
  GLsizei vertexCount;

  ProgramData() = delete;
  ProgramData(GLuint program, GLuint vao, GLsizei vertexCount) :
    program{program},
    vao{vao},
    vertexCount{vertexCount} {}
};

struct GraphicsSettings {
  std::string vertexShaderPath;
  std::string fragmentShaderPath;

  GraphicsSettings() = delete;
  GraphicsSettings(
    std::string vertexShaderPath,
    std::string fragmentShaderPath
  ) : 
    vertexShaderPath{vertexShaderPath},
    fragmentShaderPath{fragmentShaderPath} {}
};

GLuint createShader(GLenum type, std::string_view source);
std::optional<GLuint> createProgram(
  std::string_view vertexSource,
  std::string_view fragmentSource
);
std::optional<ProgramData> resetGraphics(const GraphicsSettings& settings);

#endif // GRAPHICS_HXX
