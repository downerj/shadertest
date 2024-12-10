#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "compatibility.hh"
#include "configurations.hh"
#include "core.hh"
#include "runtime.hh"
#include "window.hh"

using namespace std::string_literals;

namespace application {
std::string readShaderFromFile(const std::string& fileName) {
  std::ifstream fileIn{fileName.c_str()};
  if (fileIn.bad() or fileIn.fail()) {
    throw std::invalid_argument{"Unable to open shader input file " + fileName};
  }
  std::ostringstream textStream{};
  textStream << fileIn.rdbuf();
  fileIn.close();

  return textStream.str();
}

void printUsage() {
  std::cout << R"str(Usage: shadertest [OPTIONS] <fragmentFilePath>

OPTIONS include:
  --help            Print this help message and quit.
  --info-only       Print OpenGL version info and quit.
)str";
}

graphics::Configurations parseArguments(const std::vector<std::string>& args) {
  graphics::Configurations configs{};
  if (args.size() < 2) {
    throw std::invalid_argument{"Please provide a fragment shader file."};
  } else {
    auto argsIter{args.cbegin()};
    // Skip the first argument, which is the executable name/path.
    argsIter++;

    for (/**/; argsIter != args.cend(); ++argsIter) {
      const std::string& arg{*argsIter};
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
std::string ltrim(const std::string& s, const std::string& characters) {
  auto start{s.find_first_not_of(characters)};
  return (start == std::string::npos) ? ""s : s.substr(start);
}

std::string rtrim(const std::string& s, const std::string& characters) {
  auto end{s.find_last_not_of(characters)};
  return (end == std::string::npos) ? ""s : s.substr(0, end + 1);
}

const std::string whitespace{" \n\r\t\f\v"s};
std::string trim(const std::string& s, const std::string& characters = whitespace) {
  return rtrim(ltrim(s, characters), characters);
}

std::string preprocess(const std::string& fragmentSource, const std::filesystem::path& path) {
  const std::filesystem::path originalPath{std::filesystem::current_path()};
  std::filesystem::current_path(path);
  std::istringstream fragmentIn{fragmentSource};
  std::ostringstream fragmentOut{};
  std::string line{};
  for (int l{1}; std::getline(fragmentIn, line); ++l) {
    line = trim(line);
    if (line.starts_with("#pragma include"s)) {
      std::string segment{line.substr(15)};
      segment = trim(segment);
      segment = trim(segment, "\""s);
      std::ifstream fileIn{segment};
      if (not fileIn.good()) {
        throw std::runtime_error{
          "Fragment shader line "s + std::to_string(l) + ": Unable to open file \""s + segment + "\""s};
      }
      for (std::string fileLine{""s}; std::getline(fileIn, fileLine); fragmentOut << fileLine << '\n') {
      }
    } else {
      fragmentOut << line << '\n';
    }
  }
  std::filesystem::current_path(originalPath);
  return fragmentOut.str();
}

int main(const std::vector<std::string>& args) {
  try {
    graphics::Configurations configs{parseArguments(args)};
    if (configs.wantHelpOnly) {
      printUsage();
      return EXIT_SUCCESS;
    }
    graphics::WindowHandler windowHandler{configs};
    graphics::printInfo();
    if (configs.wantInfoOnly) {
      return EXIT_SUCCESS;
    }

    if (configs.fragmentFilePath.empty()) {
      throw std::invalid_argument{"Please specify a fragment shader path"};
    }
    const std::string fragmentRaw{readShaderFromFile(configs.fragmentFilePath)};
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

int main(int argc, char** argv) {
  const std::vector<std::string> args{argv, argv + argc};
  return application::main(args);
}
