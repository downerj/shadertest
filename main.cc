#include <fstream> // std::*fstream
#include <iostream> // std::cout, std::cerr, std::endl
#include <sstream> // std::*stringstream
#include <stdexcept> // std::exception, std::invalid_argument, std::logic_error
#include <string> // std::string, std::stoi
#include <vector> // std::vector

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void GLAPIENTRY
messageCallback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam
) {
  std::cerr << "GL CALLBACK:"
    << " type = " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
    << " severity = " << severity
    << " message = " << message
    << std::endl;
}

float vertices[] = {
  -1.0, -1.0,
  -1.0, 1.0,
  1.0, 1.0,
  1.0, -1.0,
};

unsigned int indices[] = {
  // Left triangle
  0, 1, 2,
  // Right triangle
  0, 2, 3,
};

std::string readShaderFromFile(const std::string& fileName) {
  std::ifstream fileIn(fileName.c_str());
  if (fileIn.bad() or fileIn.fail()) {
    throw std::invalid_argument("Unable to open shader input file " + fileName);
  }
  std::ostringstream textStream;
  textStream << fileIn.rdbuf();
  fileIn.close();

  return textStream.str();
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

const std::vector<GLuint> possibleGLVersions = {
  4, 6,
  4, 5,
  4, 4,
  4, 3,
  4, 2,
  4, 1,
  4, 0,
  3, 3,
  3, 2,
  3, 1,
  3, 0,
  2, 1,
  2, 0,
};

enum ProfileRequest {
  Any,
  Compat,
  Core,
};

enum VersionRequest {
  Default,
  Specific,
  Maximum,
};

struct Configs {
  std::string fragmentFilePath;
  std::string vertexSource;
  std::string fragmentSource;
  bool wantInfoOnly = false;
  enum VersionRequest versionRequest = Default;
  unsigned int wantVersionMajor = 0;
  unsigned int wantVersionMinor = 0;
  enum ProfileRequest profileRequest = Any;
  unsigned int windowWidth = 400;
  unsigned int windowHeight = 400;
  const char* windowTitle = "Shader Test";
  GLFWwindow* window = nullptr;
};

GLFWwindow* createWindow(struct Configs& configs) {
  if (configs.versionRequest == Specific) {
    if (configs.profileRequest == Core or configs.profileRequest == Compat) {
      if (configs.wantVersionMajor < 3 or (configs.wantVersionMajor == 3 and
         configs.wantVersionMinor < 3)) {
        throw std::invalid_argument("Core/compat profiles are only valid for GL>=3.3");
      }
    }
  }

  if (configs.versionRequest != Default) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, configs.wantVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, configs.wantVersionMinor);
  }
  if (configs.profileRequest == Core) {
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  } else if (configs.profileRequest == Compat) {
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  }

  return glfwCreateWindow(
    configs.windowWidth,
    configs.windowHeight,
    configs.windowTitle,
    nullptr,
    nullptr
  );
}

void printUsage() {
  std::cout << R"str(
Usage: shadertest [OPTIONS] <fragmentFilePath>

OPTIONS include:
  --help            Print this help message and quit.
  --info-only       Print OpenGL version info and quit.
  --gl=<version>    Request an OpenGL context with version X.X.
                    You can specify "max" (without quotes) to request the max version
                    available.
                    You can specify "default" (without quotes) to accept the default
                    version provided by the vendor. This is the default option.
  --core            Request an OpenGL context with a Core profile.
                    This is only valid for GL>=3.3.
  --compat          Request an OpenGL context with a Compatibility profile.
                    This is only valid for GL>=3.3.
)str";
}

void printInfo() {
  const GLubyte* glVersion = glGetString(GL_VERSION);
  const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  const GLubyte* glVendor = glGetString(GL_VENDOR);
  const GLubyte* glRenderer = glGetString(GL_RENDERER);
  std::cout << "OpenGL Version: " << glVersion << std::endl
    << "GLSL Version: " << glslVersion << std::endl
    << "OpenGL Vendor: " << glVendor << std::endl
    << "OpenGL Renderer: " << glRenderer << std::endl;
}

void parseGLVersion(const std::string& arg, struct Configs& configs) {
  std::pair<int, int> version;

  if (arg.size() < 3 or arg[1] != '.') {
    throw std::invalid_argument("GL version must be in format \"X.X\".");
  }
  unsigned int major = (unsigned int)arg[0] - 0x30;
  unsigned int minor = (unsigned int)arg[2] - 0x30;
  if (major > 9 || minor > 9) {
    throw std::invalid_argument("GL version is not a valid number.");
  }

  configs.wantVersionMajor = major;
  configs.wantVersionMinor = minor;
}

bool parseArguments(int argc, char** argv, struct Configs& configs) {
  if (argc < 2) {
    throw std::invalid_argument("Please specify a fragment shader file, or pass --info-only or --help.");
  } else {
    for (int a = 1; a < argc; a++) {
      std::string arg(argv[a]);
      if (arg == "--info-only") {
        configs.wantInfoOnly = true;
      } else if (arg == "--help") {
        printUsage();
        return false;
      } else if (arg == "--core") {
        configs.profileRequest = Core;
      } else if (arg == "--compat") {
        configs.profileRequest = Compat;
      } else if (arg.substr(0, 5) == "--gl=") {
        if (arg.substr(5) == "max") {
          configs.versionRequest = Maximum;
        } else if (arg.substr(5) == "default") {
          configs.versionRequest = Default;
        } else {
          parseGLVersion(arg.substr(5), configs);
          configs.versionRequest = Specific;
        }
      // After all switches/options, assume the rest is the file name.
      } else {
        // If we've already got a file name, then print an error.
        if (not configs.fragmentFilePath.empty()) {
          throw std::invalid_argument("Cannot have more than one fragment shader path.");
        }
        configs.fragmentFilePath = arg;
      }
    }
  }
  return true;
}

void printTryingString(const struct Configs& configs) {
  if (configs.wantVersionMajor > 0 or configs.profileRequest != Any) {
    std::cout << "Trying OpenGL";
  
    if (configs.wantVersionMajor > 0) {
      std::cout << " " << configs.wantVersionMajor << "." << configs.wantVersionMinor;
    }
    if (configs.profileRequest == Core) {
      std::cout << " Core";
    } else if (configs.profileRequest == Compat) {
      std::cout << " Compatibility";
    }
    std::cout << std::endl;
  }
}

void initializeWindow(struct Configs& configs) {
  if (not glfwInit()) {
    throw std::logic_error("Cannot initialize GLFW");
  }

  if (configs.versionRequest != Maximum) {
    configs.window = createWindow(configs);
  } else {
    for (unsigned int v = 0; v < possibleGLVersions.size(); v += 2) {
      configs.wantVersionMajor = possibleGLVersions[v];
      configs.wantVersionMinor = possibleGLVersions[v + 1];
      configs.window = createWindow(configs);
      if (configs.window) {
        break;
      }
    }
  }

  if (not configs.window) {
    throw std::logic_error("Cannot create window");
  }
  glfwMakeContextCurrent(configs.window);
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

void run(struct Configs& configs) {
  glewExperimental = GL_TRUE;
  GLenum glewStatus = glewInit();
  if (glewStatus != GLEW_OK) {
    std::cerr << glewGetErrorString(glewStatus) << std::endl;
    throw std::logic_error("Cannot initialize GLEW");
  }
  
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(messageCallback, 0);

  GLuint program = createProgram(configs.vertexSource, configs.fragmentSource);
  GLuint vertexBuffer = createBuffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  GLuint indexBuffer = createBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  GLint vertexLocation = glGetAttribLocation(program, "vertex");
  GLint resolutionLocation = glGetUniformLocation(program, "resolution");
  GLint timeLocation = glGetUniformLocation(program, "time");

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glEnableVertexAttribArray(vertexLocation);
  glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClearDepth(1.0);

  while (not glfwWindowShouldClose(configs.window)) {
    int width;
    int height;
    glfwGetFramebufferSize(configs.window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
  
    double time = glfwGetTime();
    
    glUseProgram(program);
    glUniform2f(resolutionLocation, width, height);
    glUniform1f(timeLocation, time);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);

    glfwSwapBuffers(configs.window);
    glfwPollEvents();
  }
  glfwDestroyWindow(configs.window);
  glfwTerminate();
}

int main(int argc, char** argv) {
  try {
    struct Configs configs;
    if (not parseArguments(argc, argv, configs)) {
      return EXIT_SUCCESS;
    }
    printTryingString(configs);
    initializeWindow(configs);
    printInfo();
    if (configs.wantInfoOnly) {
      return EXIT_SUCCESS;
    }

    if (configs.fragmentFilePath.empty()) {
      throw std::invalid_argument("Please specify a fragment shader path");
    }
    configs.fragmentSource = readShaderFromFile(configs.fragmentFilePath);
    configs.vertexSource = initializeVertexSource(configs.fragmentSource);
    
    run(configs);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

