#ifndef PARAMETERS_HXX
#define PARAMETERS_HXX

#include <optional>
#include <string>
#include <vector>

#include "graphics.hxx"

constexpr const char* defaultVertexSource{
#include "default.vert"
};

constexpr const char* defaultFragmentSource{
#include "default.frag"
};

struct CLIParameters {
  std::optional<std::string> vertexShaderPath{};
  std::optional<std::string> fragmentShaderPath{};
};

auto parseCLIArguments(int argc, char** argv) -> CLIParameters;
auto loadShaderSources(const CLIParameters& parameters) -> ShaderSources;

#endif // PARAMETERS_HXX
