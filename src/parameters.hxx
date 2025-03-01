#ifndef PARAMETERS_HXX
#define PARAMETERS_HXX

#include <optional>
#include <string>
#include <vector>

constexpr const char* defaultVertexSource{
#include "default.vert"
};

constexpr const char* defaultFragmentSource{
#include "default.frag"
};

constexpr const char* usageString{R"(
Usage: shadertest [OPTIONS]

Options:
    -vs <path>, --vertex-shader=<path>
        Set the vertex shader path
    -fs <path>, --fragment-shader=<path>
        Set the fragment shader path
    --echo
        Echo shaders to the console
    -h, --help
        Print this help message and quit

Notes:
    If no shaders are passed in, then a default shader will be provided.

    If a vertex shader is passed in, then a fragment shader must also be
    passed in, and vice versa.
)"};

struct CLIParameters {
  std::optional<std::string> vertexPath{};
  std::optional<std::string> fragmentPath{};
  bool echo{false};
  bool helpOnly{false};
};

struct ShaderSources {
  std::string vertex;
  std::string fragment;

  ShaderSources(std::string vertex, std::string fragment);
  ShaderSources() = delete;
};

auto parseCLIArguments(int argc, char** argv) -> CLIParameters;
auto loadShaderSources(
  const CLIParameters& parameters
) -> std::optional<ShaderSources>;

#endif // PARAMETERS_HXX
