#include "graphics.hxx"

#include <memory>
#include <stdexcept>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "debug.hxx"

#ifdef DEBUG
auto debugMessageCallbackGL(
  GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum /*severity*/,
  GLsizei /*length*/, const GLchar* message, const void* /*userParam*/
) -> void {
  LOG_ERROR("GL error: " << message << '\n');
}
#endif

ShaderData::ShaderData(
  GLuint program, GLuint vao, GLsizei indexCount, GLint timeLocation,
  GLint resolutionLocation
) : program{program}, vao{vao}, indexCount{indexCount},
    timeLocation{timeLocation}, resolutionLocation{resolutionLocation} {}

GraphicsEngine::GraphicsEngine(
  GLFWwindow* window, const std::optional<ShaderSources>& sources,
  ModelType modelType
) : window{window} {
  if (!initializeGL()) {
    throw std::runtime_error{"Failed to initialize OpenGL"};
  }
  if (sources) {
    resetWith(sources, modelType);
  }
}

GraphicsEngine::~GraphicsEngine() {
  if (!shaderData) {
    return;
  }
  glDeleteVertexArrays(1, &shaderData->vao);
  glDeleteProgram(shaderData->program);
}

auto GraphicsEngine::initializeGL() -> bool {
  if (!gladLoadGL(glfwGetProcAddress)) {
    return false;
  }
#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(debugMessageCallbackGL, nullptr);
  // if (GLAD_GL_ARB_debug_output) {
  //   LOG("GL extension GL_ARB_debug_output available\n");
  //   glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
  //   glDebugMessageCallbackARB(debugMessageCallbackGL, nullptr /*userParam*/);
  // }
  // else {
  //   LOG("GL extension GL_ARB_debug_output unavailable\n");
  // }
#endif
  return true;
}

auto GraphicsEngine::resetWith(
  const std::optional<ShaderSources>& shaderSources,
  const std::optional<ModelType>& modelType
) -> void {
  if (!shaderData && !shaderSources) {
    return;
  }
  if (!shaderSources && !modelType) {
    return;
  }

  if (shaderData) {
    if (shaderSources) {
      glDeleteProgram(shaderData->program);
    }
    if (modelType) {
      glDeleteVertexArrays(1, &shaderData->vao);
    }
  }
  const std::optional<GLuint> program{
    (shaderSources || !shaderData)
    ? createProgram(*shaderSources)
    : shaderData->program
  };
  if (modelType) {
    model = Model::createModelFromType(*modelType);
  }
  if (program) {
    const GLuint vao{createVertexArrayForModel(*program, model.get())};
    const GLint timeLocation{glGetUniformLocation(*program, "time")};
    const GLint resolutionLocation{
      glGetUniformLocation(*program, "resolution")
    };
    shaderData = {
      *program, vao, static_cast<GLsizei>(model->getIndexCount()),
      timeLocation, resolutionLocation
    };
    resetTime();
  }
}

auto GraphicsEngine::hasValidData() -> bool {
  return shaderData.has_value();
}

auto GraphicsEngine::render() -> void {
  if (!shaderData) {
    return;
  }
  int width{};
  int height{};
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glClearColor(0., .5, 1., 1.);
  glClear(GL_COLOR_BUFFER_BIT);
  if (shaderData) {
    glUseProgram(shaderData->program);
    const GLfloat elapsed = static_cast<GLfloat>(glfwGetTime()) - initialTime;
    glUniform1f(shaderData->timeLocation, elapsed);
    glUniform2i(shaderData->resolutionLocation, width, height);
    glBindVertexArray(shaderData->vao);
    glDrawElements(
      GL_TRIANGLES, shaderData->indexCount, GL_UNSIGNED_SHORT, nullptr
    );
    glBindVertexArray(0);
  }
}

auto GraphicsEngine::createShader(
  GLenum type, std::string_view source
) -> GLuint {
  GLuint shader{glCreateShader(type)};
  const GLchar* sources[]{source.data()};
  const GLint lengths[]{static_cast<GLint>(source.size())};
  glShaderSource(shader, 1, sources, lengths);
  glCompileShader(shader);
  return shader;
}

auto GraphicsEngine::createProgram(
  const ShaderSources& sources
) -> std::optional<GLuint> {
  GLuint vertexShader{createShader(GL_VERTEX_SHADER, sources.vertex)};
  GLuint fragmentShader{createShader(GL_FRAGMENT_SHADER, sources.fragment)};
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
    glGetProgramInfoLog(program, logLength, nullptr, log.data());
    std::cerr << "GL program error: " << log << '\n';
    log.clear();

    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(vertexShader, logLength, nullptr, log.data());
      std::cerr << "GL vertex shader error: " << log << '\n';
      log.clear();
    }

    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      log.resize(logLength);
      glGetShaderInfoLog(fragmentShader, logLength, nullptr, log.data());
      std::cerr << "GL fragment shader error: " << log << '\n';
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

auto GraphicsEngine::createVertexArrayForModel(
  GLuint program, const Model* model
) -> GLuint {
  const GLint positionLocation{glGetAttribLocation(program, "position")};
  GLuint vao{};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(
    GL_ARRAY_BUFFER, sizeof(GLfloat)*model->getVertexCount(),
    model->getVertices(), GL_STATIC_DRAW
  );

  GLuint indexBuffer;
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort)*model->getIndexCount(),
    model->getIndices(), GL_STATIC_DRAW
  );

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(positionLocation, 3, GL_FLOAT, false, 0, 0);
  glEnableVertexAttribArray(positionLocation);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return vao;
}

auto GraphicsEngine::resetTime() -> void {
  initialTime = static_cast<GLfloat>(glfwGetTime());
}
