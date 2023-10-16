#include <iostream> // cerr, endl
#include <stdexcept> // logic_error

#include "runtime.hh"
#include "configurations.hh"
#include "core.hh"
#include "geometry.hh"

namespace graphics {
  void run(Configurations& configs) {
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
      std::cerr << glewGetErrorString(glewStatus) << std::endl;
      throw std::logic_error("Cannot initialize GLEW");
    }
    
#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, 0);
#endif // DEBUG

    GLuint program = createProgram(configs.vertexSource, configs.fragmentSource);
    GLuint vertexBuffer = createBuffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GLuint indexBuffer = createBuffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    GLint vertexLocation = glGetAttribLocation(program, "vertex");
    GLint resolutionLocation = glGetUniformLocation(program, "resolution");
    GLint timeLocation = glGetUniformLocation(program, "time");

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(vertexLocation);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0);
    glUseProgram(program);

    while (not glfwWindowShouldClose(configs.window)) {
      int width;
      int height;
      glfwGetFramebufferSize(configs.window, &width, &height);
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);
    
      double time = glfwGetTime();
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