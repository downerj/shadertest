#include "parameters.hxx"

#include <iostream>

#include "io.hxx"

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
        parameters.vertexShaderPath = args.at(a);
      }
    } else if (arg.find("--vertex-shader=", 0) == 0) {
      std::string value{arg.substr(16)};
      if (value.length() == 0) {
        std::cerr << "Missing vertex shader path\n";
      } else {
        parameters.vertexShaderPath = value;
      }
    } else if (arg == "-fs") {
      if (a == argc - 1) {
        std::cerr << "Missing fragment shader path\n";
      } else {
        ++a;
        parameters.fragmentShaderPath = args.at(a);
      }
    } else if (arg.find("--fragment-shader=", 0) == 0) {
      std::string value{arg.substr(18)};
      if (value.length() == 0) {
        std::cerr << "Missing fragment shader path\n";
      } else {
        parameters.fragmentShaderPath = value;
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

auto loadShaderSources(const CLIParameters& parameters) -> ShaderSources {
  if (!parameters.vertexShaderPath || !parameters.fragmentShaderPath) {
    return {};
  }
  std::optional<std::string> vertexSource{
    readFile(*parameters.vertexShaderPath)
  };
  std::optional<std::string> fragmentSource{
    readFile(*parameters.fragmentShaderPath)
  };
  return {vertexSource, fragmentSource};
}
