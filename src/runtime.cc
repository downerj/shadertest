#include "runtime.hh"

#include "configurations.hh"
#include "core.hh"
#include "geometry.hh"
#include "window.hh"

namespace graphics {
void run(WindowHandler& windowHandler, const Configurations& configs) {
#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(messageCallback, 0);
#endif // DEBUG

  GLuint program{createProgram(configs.vertexSource, configs.fragmentSource)};
  GLuint vertexBuffer{
    createBuffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW)};
  GLuint indexBuffer{
    createBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW)};
  GLint vertexLocation{glGetAttribLocation(program, "vertex")};
  GLint resolutionLocation{glGetUniformLocation(program, "resolution")};
  GLint timeLocation{glGetUniformLocation(program, "time")};

  GLuint vao{};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(vertexLocation);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClearDepthf(1.f);
  glUseProgram(program);

  GLFWwindow* const window{windowHandler.getWindow()};
  double previousTime{0.};
  while (not glfwWindowShouldClose(window)) {
    int width{};
    int height{};
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform2f(resolutionLocation, width, height);
    const double time{!windowHandler.getIsAnimationPaused() ? glfwGetTime() : previousTime};
    previousTime = time;
    glUniform1f(timeLocation, time);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
}
} // namespace graphics
