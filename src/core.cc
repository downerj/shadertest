#include <iostream> // cout, cerr, endl
#include <memory> // make_unique
#include <sstream> // istringstream, ostringstream
#include <stdexcept> // invalid_argument
#include <string>  // stoi, string
#include <tuple> // tie, tuple

#include "core.hh"
#include "configurations.hh"

using GLBytes = const GLubyte*;

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
    auto buffer = std::ostringstream();
    buffer << "GL Callback: " << message;
    //std::cerr << buffer.str() << std::endl;
    throw std::runtime_error(buffer.str());
  }
#endif // DEBUG

  std::tuple<GLBytes, GLBytes, GLBytes, GLBytes> getInfo() {
    auto glVersion = glGetString(GL_VERSION);
    auto glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    auto glVendor = glGetString(GL_VENDOR);
    auto glRenderer = glGetString(GL_RENDERER);
    return {glVersion, glslVersion, glVendor, glRenderer};
  }

  void printInfo() {
    const auto [glVersion, glslVersion, glVendor, glRenderer] = getInfo();
    std::cout << "OpenGL Version: " << glVersion << std::endl
      << "GLSL Version: " << glslVersion << std::endl
      << "OpenGL Vendor: " << glVendor << std::endl
      << "OpenGL Renderer: " << glRenderer << std::endl;
  }

  std::tuple<int, int> parseGLVersion(const std::string& arg) {
    if (arg.size() < 3 or arg[1] != '.') {
      throw std::invalid_argument("GL version must be in format \"X.X\".");
    }
    const auto major = static_cast<unsigned int>(arg[0]) - 0x30u;
    const auto minor = static_cast<unsigned int>(arg[2]) - 0x30u;
    if (major > 9 || minor > 9) {
      throw std::invalid_argument("GL version is not a valid number.");
    }

    return {major, minor};
  }

  bool doesGLSLVersionUseInOut(const std::string& versionLine) {
    auto versionBuffer = std::istringstream(versionLine);
    auto input = std::string();
    // Skip the first string ("#version"), if present.
    if (versionLine.empty() or not getline(versionBuffer, input, ' ') or input != "#version") {
      return false;
    }
    // Get the version integer.
    if (not getline(versionBuffer, input, ' ')) {
      throw std::invalid_argument("Missing GLSL version \"" + versionLine + "\"");
    }
    auto isESSL = false;
    auto version = int();
    try {
      version = std::stoi(input);
      if (version < 0) {
        throw std::invalid_argument("Invalid GLSL version \"" + versionLine + "\"");
      }
    } catch (std::exception& _e) {
      throw std::invalid_argument("Invalid GLSL version \"" + versionLine + "\"");
    }
    // See if there's an "es" next.
    if (getline(versionBuffer, input, ' ')) {
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
          throw std::invalid_argument("Invalid GLSL version \"" + versionLine + "\"");
      }   
    } else {
      switch (version) {
        case 300:
        case 310:
        case 320:
          return true;
        default:
          throw std::invalid_argument("Invalid GLSL version \"" + versionLine + "\"");
      }
    }
  }

  GLuint createBuffer(GLenum target, size_t size, void* data, GLenum usage) {
    auto buffer = GLuint();
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, size, data, usage);
    glBindBuffer(target, 0);
    return buffer;
  }

  GLuint createShader(GLenum type, const std::string& source) {
    auto shader = glCreateShader(type);
    auto sources = std::make_unique<const char*[]>(1);
    sources[0] = source.c_str();
    glShaderSource(shader, 1, sources.get(), nullptr);
    glCompileShader(shader);
    return shader;
  }

  GLuint createProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    const auto vertexShader = createShader(GL_VERTEX_SHADER, vertexSource);
    const auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSource);
    const auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    auto linkStatus = GLint();
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (not linkStatus) {
      auto programLogLength = GLsizei();
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programLogLength);
      auto programLog = std::string();
      programLog.resize(programLogLength);
      glGetProgramInfoLog(program, programLogLength, &programLogLength, programLog.data());
      std::cerr << "Program linker: " << programLog << std::endl;

      auto vertexLogLength = GLsizei();
      glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &vertexLogLength);
      if (vertexLogLength > 0) {
        // GLchar* vertexLog = new GLchar[vertexLogLength];
        auto vertexLog = std::string();
        programLog.resize(vertexLogLength);
        glGetShaderInfoLog(vertexShader, vertexLogLength, &vertexLogLength, vertexLog.data());
        std::cerr << "Vertex shader compiler: " << vertexLog << std::endl;
      }

      auto fragmentLogLength = GLsizei();
      glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &fragmentLogLength);
      if (fragmentLogLength > 0) {
        auto fragmentLog = std::string();
        fragmentLog.resize(fragmentLogLength);
        glGetShaderInfoLog(fragmentShader, fragmentLogLength, &fragmentLogLength, fragmentLog.data());
        std::cerr << "Fragment shader compiler: " << fragmentLog << std::endl;
      }
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (not linkStatus) {
      throw std::invalid_argument("Error creating program");
    }

    return program;
  }

  std::string initializeVertexSource(const std::string& fragmentSource) {
    auto fragmentBuffer = std::istringstream(fragmentSource);
    auto firstLine = std::string();
    getline(fragmentBuffer, firstLine);
    auto vertexBuffer = std::ostringstream();
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
  gl_Position = vec4(vertex, 0.0, 1.0);
})str";

    return vertexBuffer.str();
  }
}
