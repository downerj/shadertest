#ifndef CORE_HH
#define CORE_HH

#include <iostream> // cout, cerr, endl
#include <sstream> // istringstream, ostringstream
#include <stdexcept> // invalid_argument
#include <string>  // stoi, string
#include <tuple> // tie, tuple

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
    std::ostringstream buffer;
    buffer << "GL Callback: " << message;
    //std::cerr << buffer.str() << std::endl;
    throw std::runtime_error(buffer.str());
  }
#endif // DEBUG

  using GLBytes = const GLubyte*;
  std::tuple<GLBytes, GLBytes, GLBytes, GLBytes> getInfo() {
    GLBytes glVersion = glGetString(GL_VERSION);
    GLBytes glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    GLBytes glVendor = glGetString(GL_VENDOR);
    GLBytes glRenderer = glGetString(GL_RENDERER);
    return {glVersion, glslVersion, glVendor, glRenderer};
  }

  void printInfo() {
    GLBytes glVersion;
    GLBytes glslVersion;
    GLBytes glVendor;
    GLBytes glRenderer;
    std::tie(glVersion, glslVersion, glVendor, glRenderer) = getInfo();
    std::cout << "OpenGL Version: " << glVersion << std::endl
      << "GLSL Version: " << glslVersion << std::endl
      << "OpenGL Vendor: " << glVendor << std::endl
      << "OpenGL Renderer: " << glRenderer << std::endl;
  }

  std::tuple<int, int> parseGLVersion(const std::string& arg) {
    if (arg.size() < 3 or arg[1] != '.') {
      throw std::invalid_argument("GL version must be in format \"X.X\".");
    }
    unsigned int major = (unsigned int)arg[0] - 0x30;
    unsigned int minor = (unsigned int)arg[2] - 0x30;
    if (major > 9 || minor > 9) {
      throw std::invalid_argument("GL version is not a valid number.");
    }

    return {major, minor};
  }

  bool doesGLSLVersionUseInOut(const std::string& versionLine) {
    std::istringstream versionBuffer(versionLine);
    std::string input;
    // Skip the first string ("#version"), if present.
    if (versionLine.empty() or not getline(versionBuffer, input, ' ') or input != "#version") {
      return false;
    }
    // Get the version integer.
    if (not getline(versionBuffer, input, ' ')) {
      throw std::invalid_argument("Missing GLSL version \"" + versionLine + "\"");
    }
    bool isESSL = false;
    int version;
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
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, size, data, usage);
    glBindBuffer(target, 0);
    return buffer;
  }

  GLuint createShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const GLchar* sources[] = {(GLchar*)source.c_str()};
    glShaderSource(shader, 1, sources, nullptr);
    glCompileShader(shader);
    return shader;
  }

  GLuint createProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (not linkStatus) {
      GLsizei programLogLength;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &programLogLength);
      GLchar* programLog = new GLchar[programLogLength];
      glGetProgramInfoLog(program, programLogLength, &programLogLength, programLog);
      std::cerr << "Program linker: " << programLog << std::endl;
      delete[] programLog;

      GLsizei vertexLogLength;
      glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &vertexLogLength);
      if (vertexLogLength > 0) {
        GLchar* vertexLog = new GLchar[vertexLogLength];
        glGetShaderInfoLog(vertexShader, vertexLogLength, &vertexLogLength, vertexLog);
        std::cerr << "Vertex shader compiler: " << vertexLog << std::endl;
        delete[] vertexLog;
      }

      GLsizei fragmentLogLength;
      glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &fragmentLogLength);
      if (fragmentLogLength > 0) {
        GLchar* fragmentLog = new GLchar[fragmentLogLength];
        glGetShaderInfoLog(fragmentShader, fragmentLogLength, &fragmentLogLength, fragmentLog);
        std::cerr << "Fragment shader compiler: " << fragmentLog << std::endl;
        delete[] fragmentLog;
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
    std::istringstream fragmentBuffer(fragmentSource);
    std::string firstLine;
    getline(fragmentBuffer, firstLine);
    std::ostringstream vertexBuffer;
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

#endif // CORE_HH
