#include "runtime.hh"

#include <iostream> // cerr, endl
#include <stdexcept> // logic_error

#include "configurations.hh"
#include "core.hh"
#include "geometry.hh"
#include "window.hh"

using namespace std;

namespace graphics {
auto run(WindowHandler& windowHandler, const Configurations& configs) -> void {
#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(messageCallback, 0);
#endif // DEBUG

  auto program{createProgram(configs.vertexSource, configs.fragmentSource)};
  auto vertexBuffer{
    createBuffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW)};
  auto indexBuffer{
    createBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW)};
  auto vertexLocation{glGetAttribLocation(program, "vertex")};
  auto resolutionLocation{glGetUniformLocation(program, "resolution")};
  auto timeLocation{glGetUniformLocation(program, "time")};

  auto vao{GLuint{}};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(vertexLocation);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClearDepthf(1.f);
  glUseProgram(program);

  const auto window{windowHandler.getWindow()};
  auto previousTime{0.};
  while (not glfwWindowShouldClose(window)) {
    auto width{int{}};
    auto height{int{}};
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform2f(resolutionLocation, width, height);
    const auto time{!windowHandler.getIsAnimationPaused() ? glfwGetTime() : previousTime};
    previousTime = time;
    glUniform1f(timeLocation, time);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
}
} // namespace graphics
