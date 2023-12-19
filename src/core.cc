#include <iostream> // cout, cerr, endl
#include <memory> // make_unique
#include <sstream> // istringstream, ostringstream
#include <stdexcept> // invalid_argument
#include <string>  // stoi, string
#include <tuple> // tie, tuple

#include "core.hh"
#include "configurations.hh"

using namespace std;
using GLBytes = const GLubyte*;

namespace graphics {
#ifdef DEBUG
  auto GLAPIENTRY messageCallback(
    GLenum /*source*/,
    GLenum /*type*/,
    GLuint /*id*/,
    GLenum /*severity*/,
    GLsizei /*length*/,
    const GLchar* message,
    const void* /*userParam*/
  ) -> void {
    auto buffer = ostringstream{};
    buffer << "GL Callback: " << message;
    throw runtime_error{buffer.str()};
  }
#endif // DEBUG

  auto getInfo() -> tuple<GLBytes, GLBytes, GLBytes, GLBytes> {
    auto glVersion = glGetString(GL_VERSION);
    auto glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    auto glVendor = glGetString(GL_VENDOR);
    auto glRenderer = glGetString(GL_RENDERER);
    return {glVersion, glslVersion, glVendor, glRenderer};
  }

  auto printInfo() -> void {
    const auto [glVersion, glslVersion, glVendor, glRenderer] = getInfo();
    cout << "OpenGL Version: " << glVersion << endl
      << "GLSL Version: " << glslVersion << endl
      << "OpenGL Vendor: " << glVendor << endl
      << "OpenGL Renderer: " << glRenderer << endl;
  }

  auto parseGLVersion(const string& arg) -> tuple<int, int> {
    if (arg.size() < 3u or arg[1u] != '.') {
      throw invalid_argument{"GL version must be in format \"X.X\"."};
    }
    try {
      const auto major = stoul(arg.substr(0u, 1u));
      const auto minor = stoul(arg.substr(2u, 1u));
      if (major > 9ul || minor > 9ul) {
        throw exception{};
      }
      return {major, minor};
    } catch (exception&) {
      throw invalid_argument{"GL version is not a valid number."};
    }
  }

  auto doesGLSLVersionUseInOut(const string& versionLine) -> bool {
    auto versionBuffer = istringstream{versionLine};
    auto input = string{};
    // Skip the first string ("#version"), if present.
    if (versionLine.empty() or not getline(versionBuffer, input, ' ') or input != "#version") {
      return false;
    }
    // Get the version integer.
    if (not getline(versionBuffer, input, ' ')) {
      throw invalid_argument{"Missing GLSL version \"" + versionLine + "\""};
    }
    using ulong = unsigned long;
    auto isESSL = false;
    auto version = ulong{};
    try {
      version = stoul(input);
    } catch (exception& _e) {
      throw invalid_argument{"Invalid GLSL version \"" + versionLine + "\""};
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
          throw invalid_argument("Invalid GLSL version \"" + versionLine + "\"");
      }   
    } else {
      switch (version) {
        case 300ul:
        case 310ul:
        case 320ul:
          return true;
        default:
          throw invalid_argument{"Invalid GLSL version \"" + versionLine + "\""};
      }
    }
  }

  auto createBuffer(GLenum target, size_t size, void* data, GLenum usage) -> GLuint {
    auto buffer = GLuint{};
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, size, data, usage);
    glBindBuffer(target, 0);
    return buffer;
  }

  auto createShader(GLenum type, const string& source) -> GLuint {
    auto shader = glCreateShader(type);
    auto sources = make_unique<const char*[]>(1u);
    sources[0] = source.c_str();
    glShaderSource(shader, 1, sources.get(), nullptr);
    glCompileShader(shader);
    return shader;
  }

  auto createProgram(const string& vertexSource, const string& fragmentSource) -> GLuint {
    const auto vertexShader = createShader(GL_VERTEX_SHADER, vertexSource);
    const auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSource);
    const auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    auto linkStatus = GLint{};
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (not linkStatus) {
      auto programLogLength = GLsizei{};
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programLogLength);
      auto programLog = vector<GLchar>{};
      programLog.resize(programLogLength);
      glGetProgramInfoLog(program, programLogLength, &programLogLength, programLog.data());
      cerr << "Program linker: " << string(programLog.begin(), programLog.end()) << endl;

      auto vertexLogLength = GLsizei{};
      glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &vertexLogLength);
      if (vertexLogLength > 0) {
        auto vertexLog = vector<GLchar>{};
        programLog.resize(vertexLogLength);
        glGetShaderInfoLog(vertexShader, vertexLogLength, &vertexLogLength, vertexLog.data());
        cerr << "Vertex shader compiler: " << string(vertexLog.begin(), vertexLog.end()) << endl;
      }

      auto fragmentLogLength = GLsizei{};
      glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &fragmentLogLength);
      if (fragmentLogLength > 0) {
        auto fragmentLog = vector<GLchar>{};
        fragmentLog.resize(fragmentLogLength);
        glGetShaderInfoLog(fragmentShader, fragmentLogLength, &fragmentLogLength, fragmentLog.data());
        cerr << "Fragment shader compiler: " << string(fragmentLog.begin(), fragmentLog.end()) << endl;
      }
    }

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (not linkStatus) {
      throw invalid_argument{"Error creating program"};
    }

    return program;
  }

  auto initializeVertexSource(const string& fragmentSource) -> string {
    auto fragmentBuffer = istringstream{fragmentSource};
    auto firstLine = string{};
    getline(fragmentBuffer, firstLine);
    auto vertexBuffer = ostringstream{};
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
}
