#include "core.hh"

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "configurations.hh"

namespace graphics {
#ifdef DEBUG
void GLAPIENTRY messageCallback(
  GLenum /*source*/,
  GLenum /*type*/,
  GLuint /*id*/,
  GLenum /*severity*/,
  GLsizei /*length*/,
  const GLchar* message,
  const void* /*userParam*/
) {
  std::cerr << "\x1b[0;33mGL message\x1b[0m: ";
  std::cerr << message << '\n';
}
#endif // DEBUG

std::tuple<std::string, std::string, std::string, std::string> getInfo() {
  // `glGetString` returns `const GLubyte*` which cannot be used to construct a
  // `std::string`. We can convert it first to `const char*` and then pass it
  // into the `std::string`'s constructor (ugly approach).
  const std::string glVersion{reinterpret_cast<const char*>(glGetString(GL_VERSION))};
  const std::string glslVersion{reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))};
  const std::string glVendor{reinterpret_cast<const char*>(glGetString(GL_VENDOR))};
  const std::string glRenderer{reinterpret_cast<const char*>(glGetString(GL_RENDERER))};
  return {glVersion, glslVersion, glVendor, glRenderer};
}

void printInfo() {
  const auto [glVersion, glslVersion, glVendor, glRenderer]{getInfo()};
  std::cout << "OpenGL Version: " << glVersion << '\n';
  std::cout << "GLSL Version: " << glslVersion << '\n';
  std::cout << "OpenGL Vendor: " << glVendor << '\n';
  std::cout << "OpenGL Renderer: " << glRenderer << '\n';
}

std::tuple<int, int> parseGLVersion(const std::string& arg) {
  if (arg.size() < 3 or arg[1] != '.') {
    throw std::invalid_argument{"GL version must be in format \"X.X\"."};
  }
  try {
    const int major{std::stoi(arg.substr(0, 1))};
    const int minor{std::stoi(arg.substr(2, 1))};
    if (major > 9 || minor > 9) {
      throw std::exception{};
    }
    return {major, minor};
  } catch (std::exception&) {
    throw std::invalid_argument{"GL version is not a valid number."};
  }
}

bool doesGLSLVersionUseInOut(const std::string& versionLine) {
  std::istringstream versionBuffer{versionLine};
  std::string input{};
  // Skip the first string ("#version"), if present.
  if (versionLine.empty() or not std::getline(versionBuffer, input, ' ') or input != "#version") {
    return false;
  }
  // Get the version integer.
  if (not std::getline(versionBuffer, input, ' ')) {
    throw std::invalid_argument{"Missing GLSL version \"" + versionLine + "\""};
  }
  bool isESSL{false};
  int version{};
  try {
    version = std::stoi(input);
  } catch (std::exception& _e) {
    throw std::invalid_argument{"Invalid GLSL version \"" + versionLine + "\""};
  }
  // See if there's an "es" next.
  if (std::getline(versionBuffer, input, ' ')) {
    if (input == "es") {
      isESSL = true;
    }
  }

  if (not isESSL) {
    switch (version) {
    // Technically #version 100 is ESSL, but for simplicity's sake we'll
    // put it here.
    case 100:
    case 110:
    case 120:
      return false;
    case 130:
    case 140:
    case 150:
    case 330:
    case 400:
    case 410:
    case 420:
    case 430:
    case 440:
    case 450:
    case 460:
      return true;
    default:
      throw std::invalid_argument{"Invalid GLSL version \"" + versionLine + "\""};
    }
  } else {
    switch (version) {
    case 300:
    case 310:
    case 320:
      return true;
    default:
      throw std::invalid_argument{"Invalid GLSL version \"" + versionLine + "\""};
    }
  }
}

GLuint createBuffer(GLenum target, size_t size, void* data, GLenum usage) {
  GLuint buffer{};
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, size, data, usage);
  glBindBuffer(target, 0);
  return buffer;
}

GLuint createShader(GLenum type, const std::string& source) {
  GLuint shader{glCreateShader(type)};
  auto sources{std::make_unique<const char*[]>(1)};
  sources[0] = source.c_str();
  glShaderSource(shader, 1, sources.get(), nullptr);
  glCompileShader(shader);
  return shader;
}

GLuint createProgram(const std::string& vertexSource, const std::string& fragmentSource) {
  const GLuint vertexShader{createShader(GL_VERTEX_SHADER, vertexSource)};
  const GLuint fragmentShader{createShader(GL_FRAGMENT_SHADER, fragmentSource)};
  const GLuint program{glCreateProgram()};
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  GLint linkStatus{};
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

  if (not linkStatus) {
    GLsizei programLogLength{};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programLogLength);
    std::vector<GLchar> programLog{};
    programLog.resize(programLogLength);
    glGetProgramInfoLog(program, programLogLength, &programLogLength, programLog.data());
    std::cerr << "Program linker: " << std::string(programLog.begin(), programLog.end());
    std::cerr << '\n';

    GLsizei vertexLogLength{};
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &vertexLogLength);
    if (vertexLogLength > 0) {
      std::vector<GLchar> vertexLog{};
      programLog.resize(vertexLogLength);
      glGetShaderInfoLog(vertexShader, vertexLogLength, &vertexLogLength, vertexLog.data());
      std::cerr << "Vertex shader compiler: ";
      std::cerr << std::string(vertexLog.begin(), vertexLog.end()) << '\n';
    }

    GLsizei fragmentLogLength{};
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &fragmentLogLength);
    if (fragmentLogLength > 0) {
      std::vector<GLchar> fragmentLog{};
      fragmentLog.resize(fragmentLogLength);
      glGetShaderInfoLog(fragmentShader, fragmentLogLength, &fragmentLogLength, fragmentLog.data());
      std::cerr << "Fragment shader compiler: ";
      std::cerr << std::string(fragmentLog.begin(), fragmentLog.end()) << '\n';
    }
  }

  glDetachShader(program, vertexShader);
  glDetachShader(program, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  if (not linkStatus) {
    throw std::invalid_argument{"Error creating program"};
  }

  return program;
}

std::string initializeVertexSource(const std::string& fragmentSource)  {
  std::istringstream fragmentBuffer{fragmentSource};
  std::string firstLine{};
  std::getline(fragmentBuffer, firstLine);
  std::ostringstream vertexBuffer{};
  if (firstLine.substr(0, 8) == "#version") {
    vertexBuffer << firstLine;
  }
  if (not doesGLSLVersionUseInOut(firstLine)) {
    vertexBuffer << R"str(
attribute vec2 vertex;)str";
  } else {
    vertexBuffer << R"str(
in vec2 vertex;)str";
  }

  vertexBuffer << R"str(
void main() {
  gl_Position = vec4(vertex, 0., 1.);
})str";

  return vertexBuffer.str();
}
} // namespace graphics
