#include "graphics.hxx"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include "io.hxx"

#ifdef DEBUG
void debugMessageCallbackGL(
  GLenum /*source*/,
  GLenum /*type*/,
  GLuint /*id*/,
  GLenum /*severity*/,
  GLsizei /*length*/,
  const GLchar* message,
  const void* /*userParam*/
) {
  LOG_ERROR("GL error: " << message << '\n');
}

void errorCallbackGLFW(int /*error*/, const char* description) {
  LOG_ERROR("GLFW error: " << description << '\n');
}
#endif

GLuint createShader(GLenum type, std::string_view source) {
  GLuint shader{glCreateShader(type)};
  const GLchar* sources[]{source.data()};
  const GLint lengths[]{static_cast<GLint>(source.size())};
  glShaderSource(shader, 1, sources, lengths);
  glCompileShader(shader);
  return shader;
}

std::optional<GLuint> createProgram(
  std::string_view vertexSource,
  std::string_view fragmentSource
) {
  GLuint vertexShader{createShader(GL_VERTEX_SHADER, vertexSource)};
  GLuint fragmentShader{createShader(GL_FRAGMENT_SHADER, fragmentSource)};
  GLuint program{glCreateProgram()};
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  GLint status{};
  glGetProgramiv(program, GL_LINK_STATUS, &status);
#ifdef DEBUG
  if (!status) {
    GLsizei logLength{};
    std::string log{};
    
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    log.resize(logLength);
    glGetProgramInfoLog(
      program,
      logLength,
      nullptr,
      log.data()
    );
    LOG_ERROR("GL program error: " << log << '\n');
    log.clear();

    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(
        vertexShader,
        logLength,
        nullptr,
        log.data()
      );
      LOG_ERROR("GL vertex shader error: " << log << '\n');
      log.clear();
    }

    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(
        fragmentShader,
        logLength,
        nullptr,
        log.data()
      );
      LOG_ERROR("GL fragment shader error: " << log << '\n');
      log.clear();
    }
  }
#endif // DEBUG
  glDetachShader(program, vertexShader);
  glDetachShader(program, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  if (!status) {
    return {};
  }
  return program;
}

std::optional<ProgramData> resetGraphics(const GraphicsSettings& settings) {
  std::optional<std::string> vertexSource{
    readFile(settings.vertexShaderPath)
  };
  if (!vertexSource) {
    return {};
  }
  std::optional<std::string> fragmentSource{
    readFile(settings.fragmentShaderPath)
  };
  if (!fragmentSource) {
    return {};
  }
  std::optional<GLuint> program{createProgram(*vertexSource, *fragmentSource)};
  GLuint vao{};
  glGenVertexArrays(1, &vao);
  return ProgramData{*program, vao, 3};
}
