#include "graphics.hxx"

#include <exception>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include "debug.hxx"
#include "models.hxx"

#ifdef DEBUG
auto debugMessageCallbackGL(
  GLenum /*source*/,
  GLenum /*type*/,
  GLuint /*id*/,
  GLenum /*severity*/,
  GLsizei /*length*/,
  const GLchar* message,
  const void* /*userParam*/
) -> void {
  LOG_ERROR("GL error: " << message << '\n');
}
#endif

constexpr const char* defaultVertexSource{
#include "default.vert"
};

constexpr const char* defaultFragmentSource{
#include "default.frag"
};

GraphicsEngine::GraphicsEngine(GLFWwindow* window) :
  GraphicsEngine(window, defaultVertexSource, defaultFragmentSource) {}

GraphicsEngine::GraphicsEngine(
  GLFWwindow* window,
  std::string_view vertexSource,
  std::string_view fragmentSource
) : window{window} {
  if (!initializeGL()) {
    throw std::runtime_error{"Failed to initialize OpenGL"};
  }
  shaderData = generateShaderData(vertexSource, fragmentSource);
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

auto GraphicsEngine::createShader(
  GLenum type,
  std::string_view source
) -> GLuint {
  GLuint shader{glCreateShader(type)};
  const GLchar* sources[]{source.data()};
  const GLint lengths[]{static_cast<GLint>(source.size())};
  glShaderSource(shader, 1, sources, lengths);
  glCompileShader(shader);
  return shader;
}

auto GraphicsEngine::createProgram(
  std::string_view vertexSource,
  std::string_view fragmentSource
) -> std::optional<GLuint> {
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
    std::cerr << "GL program error: " << log << '\n';
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
      std::cerr << "GL vertex shader error: " << log << '\n';
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

auto GraphicsEngine::generateShaderData(
  std::string_view vertexSource,
  std::string_view fragmentSource
) -> std::optional<ShaderData> {
  std::optional<GLuint> program{createProgram(vertexSource, fragmentSource)};
  if (!program) {
    return {};
  }
  const GLint positionLocation{glGetAttribLocation(*program, "position")};
  const ModelSupplier supplier{};
  const Model& model{supplier.rectangle};

  GLuint vao{};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(GLfloat)*model.getVertices().size(),
    model.getVertices().data(),
    GL_STATIC_DRAW
  );

  GLuint indexBuffer;
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    sizeof(GLshort)*model.getIndices().size(),
    model.getIndices().data(),
    GL_STATIC_DRAW
  );
  
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(positionLocation, 3, GL_FLOAT, false, 0, 0);
  glEnableVertexAttribArray(positionLocation);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  GLint timeLocation{glGetUniformLocation(*program, "time")};
  GLint resolutionLocation{glGetUniformLocation(*program, "resolution")};

  return ShaderData{
    *program,
    vao,
    static_cast<GLsizei>(model.getIndices().size()),
    timeLocation,
    resolutionLocation
  };
}

auto GraphicsEngine::render() -> void {
  int width{};
  int height{};
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glClearColor(0., .5, 1., 1.);
  glClear(GL_COLOR_BUFFER_BIT);
  if (shaderData) {
    glUseProgram(shaderData->program);
    glUniform1f(shaderData->timeLocation, static_cast<GLfloat>(glfwGetTime()));
    glUniform2i(shaderData->resolutionLocation, width, height);
    glBindVertexArray(shaderData->vao);
    glDrawElements(
      GL_TRIANGLES,
      shaderData->indexCount,
      GL_UNSIGNED_SHORT,
      nullptr
    );
    glBindVertexArray(0);
  }
  glfwSwapBuffers(window);
  glfwPollEvents();
}

GraphicsEngine::~GraphicsEngine() {
  if (!shaderData) {
    return;
  }
  glDeleteVertexArrays(1, &shaderData->vao);
  glDeleteProgram(shaderData->program);
}
