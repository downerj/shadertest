#include <algorithm> // find_if
#include <filesystem> // current_path
#include <fstream> // ifstream
#include <iostream> // cout, cerr
#include <sstream> // istringstream, ostringstream
#include <stdexcept> // exception, invalid_argument
#include <string> // string
#include <tuple> // tie
#include <vector> // vector

#include "compatibility.hh"
#include "configurations.hh"
#include "core.hh"
#include "runtime.hh"
#include "window.hh"

using namespace std::string_literals;

namespace application {
auto readShaderFromFile(const std::string& fileName) -> std::string {
  auto fileIn{std::ifstream{fileName.c_str()}};
  if (fileIn.bad() or fileIn.fail()) {
    throw std::invalid_argument{"Unable to open shader input file " + fileName};
  }
  auto textStream{std::ostringstream{}};
  textStream << fileIn.rdbuf();
  fileIn.close();

  return textStream.str();
}

auto printUsage() -> void {
  std::cout << R"str(Usage: shadertest [OPTIONS] <fragmentFilePath>

OPTIONS include:
  --help            Print this help message and quit.
  --info-only       Print OpenGL version info and quit.
)str";
}

auto parseArguments(const std::vector<std::string>& args) -> graphics::Configurations {
  auto configs{graphics::Configurations{}};
  if (args.size() < 2u) {
    throw std::invalid_argument{"Please provide a fragment shader file."};
  } else {
    auto argsIter{args.cbegin()};
    // Skip the first argument, which is the executable name/path.
    argsIter++;

    for (/**/; argsIter != args.cend(); ++argsIter) {
      const auto& arg{*argsIter};
      if (arg == "--info-only") {
        configs.wantInfoOnly = true;
      } else if (arg == "--help") {
        configs.wantHelpOnly = true;
        break;
      } else {
        // If we've already got a file name, then print an error.
        if (not configs.fragmentFilePath.empty()) {
          throw std::invalid_argument{"Cannot have more than one fragment shader path."};
        }
        configs.fragmentFilePath = arg;
      }
    }
  }
  return configs;
}

// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
auto ltrim(const std::string& s, const std::string& characters) -> std::string {
  auto start{s.find_first_not_of(characters)};
  return (start == std::string::npos) ? ""s : s.substr(start);
}

auto rtrim(const std::string& s, const std::string& characters) -> std::string {
  auto end{s.find_last_not_of(characters)};
  return (end == std::string::npos) ? ""s : s.substr(0u, end + 1u);
}

const auto whitespace{" \n\r\t\f\v"s};
auto trim(const std::string& s, const std::string& characters = whitespace) -> std::string {
  return rtrim(ltrim(s, characters), characters);
}

auto preprocess(const std::string& fragmentSource, const std::filesystem::path& path) -> std::string {
  const auto originalPath{std::filesystem::current_path()};
  std::filesystem::current_path(path);
  auto fragmentIn{std::istringstream{fragmentSource}};
  auto fragmentOut{std::ostringstream{}};
  auto line{std::string{}};
  for (auto l{1u}; std::getline(fragmentIn, line); ++l) {
    line = trim(line);
    if (line.starts_with("#pragma include"s)) {
      auto segment{line.substr(15)};
      segment = trim(segment);
      segment = trim(segment, "\""s);
      auto fileIn{std::ifstream{segment}};
      if (not fileIn.good()) {
        throw std::runtime_error{
          "Fragment shader line "s + std::to_string(l) + ": Unable to open file \""s + segment + "\""s};
      }
      for (auto fileLine{""s}; std::getline(fileIn, fileLine); fragmentOut << fileLine << '\n') {
      }
    } else {
      fragmentOut << line << '\n';
    }
  }
  std::filesystem::current_path(originalPath);
  return fragmentOut.str();
}

auto main(const std::vector<std::string>& args) -> int {
  try {
    auto configs{parseArguments(args)};
    if (configs.wantHelpOnly) {
      printUsage();
      return EXIT_SUCCESS;
    }
    auto windowHandler{graphics::WindowHandler{configs}};
    graphics::printInfo();
    if (configs.wantInfoOnly) {
      return EXIT_SUCCESS;
    }

    if (configs.fragmentFilePath.empty()) {
      throw std::invalid_argument{"Please specify a fragment shader path"};
    }
    const auto fragmentRaw{readShaderFromFile(configs.fragmentFilePath)};
    const auto fragmentDir{std::filesystem::path{configs.fragmentFilePath}.parent_path()};
    configs.fragmentSource = preprocess(fragmentRaw, fragmentDir);
    configs.vertexSource = graphics::initializeVertexSource(configs.fragmentSource);

    graphics::run(windowHandler, configs);
  } catch (std::exception& e) {
    std::cerr << "\x1b[0;31mError\x1b[0m: ";
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
} // namespace application

auto main(int argc, char** argv) -> int {
  const auto args{std::vector<std::string>{argv, argv + argc}};
  return application::main(args);
}
