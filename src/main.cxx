#include <cstdlib>

#include "debug.hxx"
#include "graphics.hxx"
#include "io.hxx"

auto main(int, char**) -> int {
  GLFWwindow* window{initializeWindow()};
  if (!window) {
    std::exit(EXIT_FAILURE);
  }
  std::optional<ShaderData> shaderData{
    generateShaderData(defaultVertexSource, defaultFragmentSource)
  };
  if (!shaderData) {
    std::exit(EXIT_FAILURE);
  }
  LOG("Running...\n");
  while (!glfwWindowShouldClose(window)) {
    render(window, *shaderData);
  }
  cleanup(window, *shaderData);
  LOG("Goodbye.\n");
}
