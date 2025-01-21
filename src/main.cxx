#include <cstdlib>
#include <exception>
#include <string_view>

#include "debug.hxx"
#include "graphics.hxx"
#include "parameters.hxx"
#include "window.hxx"

auto main(int argc, char** argv) -> int {
  CLIParameters parameters{parseCLIArguments(argc, argv)};
  ShaderSources sources{};
  if (!parameters.vertexShaderPath && !parameters.fragmentShaderPath) {
    sources = {defaultVertexSource, defaultFragmentSource};
  } else if (parameters.vertexShaderPath && parameters.fragmentShaderPath) {
    sources = loadShaderSources(parameters);
  } else {
    std::cerr << "Please pass in both a vertex shader and a fragment shader\n";
  }

  try {
    WindowOwner windowOwner{};
    GraphicsEngine graphics{
      windowOwner.getWindow(),
      sources
    };
    LOG("Running...\n");
    while (windowOwner.isActive()) {
      graphics.render();
    }
    LOG("Goodbye.\n");
  } catch (std::exception& ex) {
    std::cerr << ex.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}
