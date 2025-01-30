#include "parameters.hxx"

#include <iostream>

#include "io.hxx"

ShaderSources::ShaderSources(
  std::string vertex_, std::string fragment_
) : vertex{vertex_}, fragment{fragment_} {}

auto parseCLIArguments(int argc, char** argv) -> CLIParameters {
  std::vector<std::string> args{argv, argv + argc};
  CLIParameters parameters{};
  for (int a{1}; a < argc; ++a) {
    const std::string arg{args.at(a)};
    if (arg == "-vs") {
      if (a == argc - 1) {
        std::cerr << "Missing vertex shader path\n";
      } else {
        ++a;
        parameters.vertexPath = args.at(a);
      }
    } else if (arg.find("--vertex-shader=", 0) == 0) {
      std::string value{arg.substr(16)};
      if (value.length() == 0) {
        std::cerr << "Missing vertex shader path\n";
      } else {
        parameters.vertexPath = value;
      }
    } else if (arg == "-fs") {
      if (a == argc - 1) {
        std::cerr << "Missing fragment shader path\n";
      } else {
        ++a;
        parameters.fragmentPath = args.at(a);
      }
    } else if (arg.find("--fragment-shader=", 0) == 0) {
      std::string value{arg.substr(18)};
      if (value.length() == 0) {
        std::cerr << "Missing fragment shader path\n";
      } else {
        parameters.fragmentPath = value;
      }
    } else if (arg.find("--echo", 0) == 0) {
      parameters.echo = true;
    } else if (arg == "-h" || arg.find("--help", 0) == 0) {
      parameters.helpOnly = true;
    } else {
      std::cerr << "Unknown argument \"" << arg << "\"\n";
    }
  }
  return parameters;
}

auto loadShaderSources(
  const CLIParameters& parameters
) -> std::optional<ShaderSources> {
  if (!parameters.vertexPath || !parameters.fragmentPath) {
    return {};
  }
  std::optional<std::string> vertex{
    readFile(*parameters.vertexPath)
  };
  std::optional<std::string> fragment{
    readFile(*parameters.fragmentPath)
  };
  if (vertex && fragment) {
    return {{*vertex, *fragment}};
  }
  return {};
}
