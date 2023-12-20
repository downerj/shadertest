#include <fstream> // ifstream
#include <iostream> // cout, cerr, endl
#include <sstream> // ostringstream
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
    auto fileIn = ifstream{fileName.c_str()};
    if (fileIn.bad() or fileIn.fail()) {
      throw invalid_argument{"Unable to open shader input file " + fileName};
    }
    auto textStream = ostringstream{};
    textStream << fileIn.rdbuf();
    fileIn.close();

    return textStream.str();
  }

  auto printUsage() -> void {
    cout << R"str(Usage: shadertest [OPTIONS] <fragmentFilePath>

OPTIONS include:
  --help            Print this help message and quit.
  --info-only       Print OpenGL version info and quit.
  --gl=<version>    Request an OpenGL context with version X.X.
                    You can specify "max" (without quotes) to request the max version
                    available.
                    You can specify "default" (without quotes) to accept the default
                    version provided by the vendor. This is the default option.
  --core            Request an OpenGL context with a Core profile.
                    This is only valid for GL>=3.3.
  --compat          Request an OpenGL context with a Compatibility profile.
                    This is only valid for GL>=3.3.
)str";
  }

  auto parseArguments(const vector<string>& args) -> graphics::Configurations {
    auto configs = graphics::Configurations{};
    if (args.size() < 2u) {
      throw invalid_argument{"Please specify a fragment shader file, or pass --info-only or --help."};
    } else {
      auto argsIter = args.cbegin();
      // Skip the first argument, which is the executable name/path.
      argsIter++;

      for (/**/; argsIter != args.cend(); ++argsIter) {
        const auto& arg = *argsIter;
        if (arg == "--info-only") {
          configs.wantInfoOnly = true;
        } else if (arg == "--help") {
          configs.wantHelpOnly = true;
          break;
        } else if (arg == "--core") {
          configs.profileRequest = graphics::ProfileRequest::Core;
        } else if (arg == "--compat") {
          configs.profileRequest = graphics::ProfileRequest::Compat;
        } else if (arg.substr(0, 5) == "--gl=") {
          if (arg.substr(5) == "max") {
            configs.versionRequest = graphics::VersionRequest::Maximum;
          } else if (arg.substr(5) == "default") {
            configs.versionRequest = graphics::VersionRequest::Default;
          } else {
            tie(configs.wantVersionMajor, configs.wantVersionMinor) = graphics::parseGLVersion(arg.substr(5));
            configs.versionRequest = graphics::VersionRequest::Specific;
          }
        // After all switches/options, assume the rest is the file name.
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

  auto printTryingString(const graphics::Configurations& configs) -> void {
    if (configs.wantVersionMajor > 0 or configs.profileRequest != graphics::ProfileRequest::Any) {
      cout << "Trying OpenGL";
    
      if (configs.wantVersionMajor > 0) {
        cout << " " << configs.wantVersionMajor << "." << configs.wantVersionMinor;
      }
      if (configs.profileRequest == graphics::ProfileRequest::Compat) {
        cout << " Compatibility";
      } else if (configs.profileRequest == graphics::ProfileRequest::Core) {
        cout << " Core";
      }
      cout << endl;
    }
  }

  auto main(const vector<string>& args) -> int {
    try {
      auto configs = parseArguments(args);
      if (configs.wantHelpOnly) {
        printUsage();
        return EXIT_SUCCESS;
      }
      printTryingString(configs);
      graphics::initializeWindow(configs);
      graphics::printInfo();
      if (configs.wantInfoOnly) {
        return EXIT_SUCCESS;
      }

      if (configs.fragmentFilePath.empty()) {
        throw invalid_argument{"Please specify a fragment shader path"};
      }
      configs.fragmentSource = readShaderFromFile(configs.fragmentFilePath);
      configs.vertexSource = graphics::initializeVertexSource(configs.fragmentSource);

      graphics::run(configs);
    } catch (exception& e) {
      cerr << "\x1b[0;31m" << "Error" "\x1b[0m" << ": " << e.what() << endl;
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }
}

auto main(int argc, char** argv) -> int {
  const auto args = vector<string>{argv, argv + argc};
  return application::main(args);
}
