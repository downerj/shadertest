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
  std::optional<std::string> vertexShaderPath{};
  std::optional<std::string> fragmentShaderPath{};
  bool echo{false};
  bool helpOnly{false};
};

auto parseCLIArguments(int argc, char** argv) -> CLIParameters;
auto loadShaderSources(const CLIParameters& parameters) -> ShaderSources;

#endif // PARAMETERS_HXX
