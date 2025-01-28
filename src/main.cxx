#include <cstdlib>
#include <exception>
#include <iostream>
#include <string_view>

#include "debug.hxx"
#include "graphics.hxx"
#include "parameters.hxx"
#include "window.hxx"

auto echoSources(const ShaderSources& sources) -> void {
  std::cout << "##### BEGIN VERTEX SHADER #####\n";
  std::cout << sources.vertex << '\n';
  std::cout << "##### END VERTEX SHADER #####\n\n";
  std::cout << "##### BEGIN FRAGMENT SHADER #####\n";
  std::cout << sources.fragment << '\n';
  std::cout << "##### END FRAGMENT SHADER #####\n";
}

auto main(int argc, char** argv) -> int {
  CLIParameters parameters{parseCLIArguments(argc, argv)};
  if (parameters.helpOnly) {
    std::cout << usageString << '\n';
    std::exit(EXIT_SUCCESS);
  }
  std::optional<ShaderSources> sources{};
  if (!parameters.vertexPath && !parameters.fragmentPath) {
    sources = {defaultVertexSource, defaultFragmentSource};
  } else if (parameters.vertexPath && parameters.fragmentPath) {
    sources = loadShaderSources(parameters);
  } else {
    std::cerr << "Please pass in both a vertex shader and a fragment shader\n";
  }

  try {
    WindowOwner windowOwner{};
    GraphicsEngine graphics{
      windowOwner.getWindow(), sources, windowOwner.getActions().modelType
    };
    if (parameters.echo && sources) {
      echoSources(*sources);
    }
    WindowActions& actions{windowOwner.getActions()};
    bool paused{false};
    while (windowOwner.isActive()) {
      if (actions.changeModelType) {
        graphics.resetWith({}, actions.modelType);
      } else if (actions.closeWindow) {
        windowOwner.closeWindow();
      } else if (actions.resetWindowSize) {
        windowOwner.resetWindowSize();
      } else if (actions.pauseResume) {
        paused = !paused;
      }
      actions.reset();
      if (!paused) {
        graphics.render();
      }
      windowOwner.update();
    }
    std::cout << "Goodbye.\n";
  } catch (std::exception& ex) {
    std::cerr << ex.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}
