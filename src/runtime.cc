#include <iostream> // cerr, endl
#include <stdexcept> // logic_error

#include "runtime.hh"
#include "configurations.hh"
#include "core.hh"
#include "geometry.hh"

using namespace std;

namespace graphics {
  auto run(Configurations& configs) -> void {
    glewExperimental = GL_TRUE;
    auto glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
      cerr << glewGetErrorString(glewStatus) << endl;
      throw logic_error{"Cannot initialize GLEW"};
    }
    
#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, 0);
#endif // DEBUG

    auto program = createProgram(configs.vertexSource, configs.fragmentSource);
    auto vertexBuffer = createBuffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    auto indexBuffer = createBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
    auto vertexLocation = glGetAttribLocation(program, "vertex");
    auto resolutionLocation = glGetUniformLocation(program, "resolution");
    auto timeLocation = glGetUniformLocation(program, "time");

    auto vao = GLuint{};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(vertexLocation);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClearDepth(1.f);
    glUseProgram(program);

    while (not glfwWindowShouldClose(configs.window)) {
      auto width = int{};
      auto height = int{};
      glfwGetFramebufferSize(configs.window, &width, &height);
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);
    
      const auto time = glfwGetTime();
      glUniform2f(resolutionLocation, width, height);
      glUniform1f(timeLocation, time);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);

      glfwSwapBuffers(configs.window);
      glfwPollEvents();
    }
    glfwDestroyWindow(configs.window);
    glfwTerminate();
  }
}