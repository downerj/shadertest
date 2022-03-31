#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shaders.hh"

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
  //const GLchar* sources[] = {"void main(){}"};
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

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

int main(int argc, char** argv) {
  try {
    std::string fragmentShaderSource;
    if (argc < 2) {
      std::cout << "Using default fragment shader" << std::endl;
      fragmentShaderSource = fragmentDefaultShaderSource;
    } else {
      fragmentShaderSource = readShaderFromFile(argv[1]);
    }

    if (not glfwInit()) {
      std::cerr << "Cannot initialize GLFW" << std::endl;
      return EXIT_FAILURE;
    }
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Shader Test", nullptr, nullptr);
    if (not window) {
      std::cerr << "Cannot create window" << std::endl;
      return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
   
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
      std::cerr << "Cannot initialize GLEW" << std::endl;
      std::cerr << glewGetErrorString(glewStatus) << std::endl;
      return EXIT_FAILURE;
    }

    GLuint program = createProgram(vertexShaderSource, fragmentShaderSource);
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

    while (not glfwWindowShouldClose(window)) {
      int width;
      int height;
      glfwGetFramebufferSize(window, &width, &height);
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);
    
      double time = glfwGetTime();
      
      glUseProgram(program);
      glUniform2f(resolutionLocation, width, height);
      glUniform1f(timeLocation, time);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);
      
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

