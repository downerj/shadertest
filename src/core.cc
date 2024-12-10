#include "core.hh"

#include <iostream> // cout, cerr
#include <memory> // make_unique
#include <sstream> // istringstream, ostringstream
#include <stdexcept> // invalid_argument
#include <string> // stoi, string
#include <tuple> // tie, tuple
#include <vector> // vector

#include "configurations.hh"

namespace graphics {
#ifdef DEBUG
auto GLAPIENTRY messageCallback(
  GLenum, // source
  GLenum, // type
  GLuint, // id
  GLenum, // severity
  GLsizei, // length
  const GLchar* message,
  const void* // userParam
) -> void {
  std::cerr << "\x1b[0;33mGL message\x1b[0m: ";
  std::cerr << message << '\n';
}
#endif // DEBUG

auto getInfo() -> std::tuple<std::string, std::string, std::string, std::string> {
  // `glGetString` returns `const GLubyte*` which cannot be used to construct a
  // `std::string`. We can convert it first to `const char*` and then pass it
  // into the `std::string`'s constructor (ugly approach).
  auto glVersion{std::string{reinterpret_cast<const char*>(glGetString(GL_VERSION))}};
  auto glslVersion{std::string{reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))}};
  auto glVendor{std::string{reinterpret_cast<const char*>(glGetString(GL_VENDOR))}};
  auto glRenderer{std::string{reinterpret_cast<const char*>(glGetString(GL_RENDERER))}};
  return {glVersion, glslVersion, glVendor, glRenderer};
}

auto printInfo() -> void {
  const auto [glVersion, glslVersion, glVendor, glRenderer]{getInfo()};
  std::cout << "OpenGL Version: " << glVersion << '\n';
  std::cout << "GLSL Version: " << glslVersion << '\n';
  std::cout << "OpenGL Vendor: " << glVendor << '\n';
  std::cout << "OpenGL Renderer: " << glRenderer << '\n';
}

auto parseGLVersion(const std::string& arg) -> std::tuple<int, int> {
  if (arg.size() < 3u or arg[1u] != '.') {
    throw std::invalid_argument{"GL version must be in format \"X.X\"."};
  }
  try {
    const auto major{std::stoul(arg.substr(0u, 1u))};
    const auto minor{std::stoul(arg.substr(2u, 1u))};
    if (major > 9ul || minor > 9ul) {
      throw std::exception{};
    }
    return {major, minor};
  } catch (std::exception&) {
    throw std::invalid_argument{"GL version is not a valid number."};
  }
}

auto doesGLSLVersionUseInOut(const std::string& versionLine) -> bool {
  auto versionBuffer{std::istringstream{versionLine}};
  auto input{std::string{}};
  // Skip the first string ("#version"), if present.
  if (versionLine.empty() or not std::getline(versionBuffer, input, ' ') or input != "#version") {
    return false;
  }
  // Get the version integer.
  if (not std::getline(versionBuffer, input, ' ')) {
    throw std::invalid_argument{"Missing GLSL version \"" + versionLine + "\""};
  }
  auto isESSL{false};
  auto version{0ul};
  try {
    version = std::stoul(input);
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
    case 100ul:
    case 110ul:
    case 120ul:
      return false;
    case 130ul:
    case 140ul:
    case 150ul:
    case 330ul:
    case 400ul:
    case 410ul:
    case 420ul:
    case 430ul:
    case 440ul:
    case 450ul:
    case 460ul:
      return true;
    default:
      throw std::invalid_argument{"Invalid GLSL version \"" + versionLine + "\""};
    }
  } else {
    switch (version) {
    case 300ul:
    case 310ul:
    case 320ul:
      return true;
    default:
      throw std::invalid_argument{"Invalid GLSL version \"" + versionLine + "\""};
    }
  }
}

auto createBuffer(GLenum target, size_t size, void* data, GLenum usage) -> GLuint {
  auto buffer{GLuint{}};
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, size, data, usage);
  glBindBuffer(target, 0);
  return buffer;
}

auto createShader(GLenum type, const std::string& source) -> GLuint {
  auto shader{glCreateShader(type)};
  auto sources{std::make_unique<const char*[]>(1u)};
  sources[0] = source.c_str();
  glShaderSource(shader, 1, sources.get(), nullptr);
  glCompileShader(shader);
  return shader;
}

auto createProgram(const std::string& vertexSource, const std::string& fragmentSource) -> GLuint {
  const auto vertexShader{createShader(GL_VERTEX_SHADER, vertexSource)};
  const auto fragmentShader{createShader(GL_FRAGMENT_SHADER, fragmentSource)};
  const auto program{glCreateProgram()};
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  auto linkStatus{GLint{}};
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

  if (not linkStatus) {
    auto programLogLength{GLsizei{}};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programLogLength);
    auto programLog{std::vector<GLchar>{}};
    programLog.resize(programLogLength);
    glGetProgramInfoLog(program, programLogLength, &programLogLength, programLog.data());
    std::cerr << "Program linker: " << std::string(programLog.begin(), programLog.end());
    std::cerr << '\n';

    auto vertexLogLength{GLsizei{}};
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &vertexLogLength);
    if (vertexLogLength > 0) {
      auto vertexLog{std::vector<GLchar>{}};
      programLog.resize(vertexLogLength);
      glGetShaderInfoLog(vertexShader, vertexLogLength, &vertexLogLength, vertexLog.data());
      std::cerr << "Vertex shader compiler: ";
      std::cerr << std::string(vertexLog.begin(), vertexLog.end()) << '\n';
    }

    auto fragmentLogLength{GLsizei{}};
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &fragmentLogLength);
    if (fragmentLogLength > 0) {
      auto fragmentLog{std::vector<GLchar>{}};
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

auto initializeVertexSource(const std::string& fragmentSource) -> std::string {
  auto fragmentBuffer{std::istringstream{fragmentSource}};
  auto firstLine{std::string{}};
  std::getline(fragmentBuffer, firstLine);
  auto vertexBuffer{std::ostringstream{}};
  if (firstLine.substr(0u, 8u) == "#version") {
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
