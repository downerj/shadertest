#include <cstdlib>
#include <exception>
#include <iostream>
#include <string_view>

#include "debug.hxx"
#include "graphics.hxx"
#include "parameters.hxx"
#include "window.hxx"

auto main(int argc, char** argv) -> int {
  CLIParameters parameters{parseCLIArguments(argc, argv)};
  if (parameters.helpOnly) {
    std::cout << usageString << '\n';
    std::exit(EXIT_SUCCESS);
  }
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
    if (parameters.echo && sources.vertexSource && sources.fragmentSource) {
      std::cout << "##### BEGIN VERTEX SHADER #####\n";
      std::cout << *sources.vertexSource << '\n';
      std::cout << "##### END VERTEX SHADER #####\n\n";

      std::cout << "##### BEGIN FRAGMENT SHADER #####\n";
      std::cout << *sources.fragmentSource << '\n';
      std::cout << "##### END FRAGMENT SHADER #####\n";
    }
    while (windowOwner.isActive()) {
      graphics.render();
    }
    LOG("Goodbye.\n");
  } catch (std::exception& ex) {
    std::cerr << ex.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}
