#include <algorithm> // find_if
#include <filesystem> // current_path
#include <fstream> // ifstream
#include <iostream> // cout, cerr, endl
#include <sstream> // istringstream, ostringstream
#include <stdexcept> // exception, invalid_argument
#include <string> // string
#include <tuple> // tie

#include "compatibility.hh"
#include "configurations.hh"
#include "core.hh"
#include "runtime.hh"
#include "window.hh"

using namespace std;

namespace application {
auto readShaderFromFile(const string& fileName) -> string {
  auto fileIn{ifstream{fileName.c_str()}};
  if (fileIn.bad() or fileIn.fail()) {
    throw invalid_argument{"Unable to open shader input file " + fileName};
  }
  auto textStream{ostringstream{}};
  textStream << fileIn.rdbuf();
  fileIn.close();

  return textStream.str();
}

auto printUsage() -> void {
  cout << R"str(Usage: shadertest [OPTIONS] <fragmentFilePath>

OPTIONS include:
  --help            Print this help message and quit.
  --info-only       Print OpenGL version info and quit.
)str";
}

auto parseArguments(const vector<string>& args) -> graphics::Configurations {
  auto configs{graphics::Configurations{}};
  if (args.size() < 2u) {
    throw invalid_argument{"Please provide a fragment shader file."};
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
          throw invalid_argument{"Cannot have more than one fragment shader path."};
        }
        configs.fragmentFilePath = arg;
      }
    }
  }
  return configs;
}

// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
auto ltrim(const string& s, const string& characters) -> string {
  auto start{s.find_first_not_of(characters)};
  return (start == string::npos) ? ""s : s.substr(start);
}

auto rtrim(const string& s, const string& characters) -> string {
  auto end{s.find_last_not_of(characters)};
  return (end == string::npos) ? ""s : s.substr(0u, end + 1u);
}

const auto whitespace{" \n\r\t\f\v"s};
auto trim(const string& s, const string& characters = whitespace) -> string {
  return rtrim(ltrim(s, characters), characters);
}

auto preprocess(const string& fragmentSource, const filesystem::path& path) -> string {
  const auto originalPath{filesystem::current_path()};
  filesystem::current_path(path);
  auto fragmentIn{istringstream{fragmentSource}};
  auto fragmentOut{ostringstream{}};
  auto line{string{}};
  for (auto l{1u}; getline(fragmentIn, line); ++l) {
    line = trim(line);
    if (line.starts_with("#pragma include"s)) {
      auto segment{line.substr(15)};
      segment = trim(segment);
      segment = trim(segment, "\""s);
      auto fileIn{ifstream{segment}};
      if (not fileIn.good()) {
        throw runtime_error{
          "Fragment shader line "s + to_string(l) + ": Unable to open file \""s + segment + "\""s};
      }
      for (auto fileLine{""s}; getline(fileIn, fileLine); fragmentOut << fileLine << endl) {
      }
    } else {
      fragmentOut << line << endl;
    }
  }
  filesystem::current_path(originalPath);
  return fragmentOut.str();
}

auto main(const vector<string>& args) -> int {
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
      throw invalid_argument{"Please specify a fragment shader path"};
    }
    const auto fragmentRaw{readShaderFromFile(configs.fragmentFilePath)};
    const auto fragmentDir{filesystem::path{configs.fragmentFilePath}.parent_path()};
    configs.fragmentSource = preprocess(fragmentRaw, fragmentDir);
    configs.vertexSource = graphics::initializeVertexSource(configs.fragmentSource);

    graphics::run(windowHandler, configs);
  } catch (exception& e) {
    cerr << "\x1b[0;31mError\x1b[0m: ";
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
} // namespace application

auto main(int argc, char** argv) -> int {
  const auto args{vector<string>{argv, argv + argc}};
  return application::main(args);
}
