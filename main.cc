#include <fstream> // std::
#include <iostream> // std::cout, std::cerr, std::endl
#include <sstream> // std::
#include <stdexcept> // std::exception
#include <string> // std::string
#include <vector> // std::vector

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

std::string readShaderFromFile(const char* fileName) {
  if (not fileName) {
    throw std::invalid_argument("No shader input file name provided");
  }

  std::ifstream fileIn(fileName);
  if (fileIn.bad()) {
    throw std::invalid_argument("Unable to open shader input file");
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

struct Configs {
  std::string fragmentFilePath;
  bool wantInfo = false;
  bool wantHelp = false;
  unsigned int versionMajor = 0;
  unsigned int versionMinor = 0;
  bool wantCore = false;
  unsigned int windowWidth = 400;
  unsigned int windowHeight = 400;
  const char* windowTitle = "Shader Test";
};

GLFWwindow* createWindow(struct Configs& configs) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, configs.versionMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, configs.versionMinor);
  if (configs.wantCore) {
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
  --info            Print OpenGL version info and quit.
Version OPTIONS are mutually exclusive:
  --gl=<version>    Use an OpenGL context with version X.X.
  --core            Use an OpenGL Core Core context with version X.X.
  --essl=<version>  Use GLSL ES (ESSL) version XXX instead of GLSL.
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

bool parseGLVersion(const std::string& arg, struct Configs& configs) {
  std::pair<int, int> version;

  if (arg.size() < 4 or arg[0] != '=' or arg[2] != '.') {
    std::cerr << "GL version must be in format \"=X.X\"." << std::endl;
    return false;
  }
  unsigned int major = (unsigned int)arg[1] - 0x30;
  unsigned int minor = (unsigned int)arg[3] - 0x30;
  if (major > 9 || minor > 9) {
    std::cerr << "GL version is not a valid number." << std::endl;
    return false;
  }

  configs.versionMajor = major;
  configs.versionMinor = minor;
  return true;
}

int main(int argc, char** argv) {
  try {
    struct Configs configs;
    if (argc < 2) {
      std::cerr << "Please specify a fragment shader file." << std::endl;
      printUsage();
      return EXIT_FAILURE;
    } else {
      for (int a = 1; a < argc; a++) {
        std::string arg(argv[a]);
        if (arg == "--info") {
          configs.wantInfo = true;
          break;
        }
        if (arg == "--help") {
          configs.wantHelp = true;
          break;
        }
        if (arg.substr(0, 9) == "--gl+essl") {
          if (not parseGLVersion(arg.substr(9), configs)) {
            return EXIT_FAILURE;
          }
          configs.wantCore = false;
          configs.wantESSL = true;
        } else if (arg.substr(0, 8) == "--glcore") {
          if (not parseGLVersion(arg.substr(8), configs)) {
            return EXIT_FAILURE;
          }
          configs.wantCore = true;
          configs.wantESSL = false;
        } else if (arg.substr(0, 4) == "--gl") {
          if (not parseGLVersion(arg.substr(4), configs)) {
            return EXIT_FAILURE;
          }
          configs.wantCore = false;
          configs.wantESSL = false;
        }
      }
    }
    
    if (configs.wantHelp) {
      printUsage();
      return EXIT_SUCCESS;
    }

    if (not configs.wantInfo) {
      std::cout << "Trying OpenGL "
        << configs.versionMajor << "." << configs.versionMinor;
      if (configs.wantCore) {
        std::cout << " Core";
      } else if (configs.wantESSL) {
        std::cout << " + ESSL";
      }
      std::cout << std::endl;
    }

    //std::string fragmentShaderSource = readShaderFromFile(argv[1]);

    if (not glfwInit()) {
      std::cerr << "Cannot initialize GLFW" << std::endl;
      return EXIT_FAILURE;
    }

    GLFWwindow* window;
    if (not configs.wantInfo && configs.versionMajor > 0) {
      window = createWindow(configs);
    } else {
      for (unsigned int v = 0; v < possibleGLVersions.size(); v += 2) {
        configs.versionMajor = possibleGLVersions[v];
        configs.versionMinor = possibleGLVersions[v + 1];
        window = createWindow(configs);
        if (window) {
          break;
        }
      }
    }

    if (not window) {
      std::cerr << "Cannot create window" << std::endl;
      return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    printInfo();
    if (configs.wantInfo) {
      return EXIT_SUCCESS;
    }

    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
      std::cerr << "Cannot initialize GLEW" << std::endl;
      std::cerr << glewGetErrorString(glewStatus) << std::endl;
      return EXIT_FAILURE;
    }

    /*
    GLuint program = createProgram(vertexSourceDefault, fragmentShaderSource);
    GLuint vertexBuffer = createBuffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GLuint indexBuffer = createBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    GLint vertexLocation = glGetAttribLocation(program, "vertex");
    GLint resolutionLocation = glGetUniformLocation(program, "resolution");
    GLint timeLocation = glGetUniformLocation(program, "time");

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    */

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0);

    while (not glfwWindowShouldClose(window)) {
      int width;
      int height;
      glfwGetFramebufferSize(window, &width, &height);
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);
    
      double time = glfwGetTime();
      
      /*
      glUseProgram(program);
      glUniform2f(resolutionLocation, width, height);
      glUniform1f(timeLocation, time);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
      */

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

