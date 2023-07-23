#include <fstream> // ifstream
#include <iostream> // cout, cerr, endl
#include <sstream> // ostringstream
#include <stdexcept> // exception, invalid_argument
#include <string> // string
#include <tuple> // tie

#include <graphics/Compatibility.hh>
#include <graphics/Configurations.hh>
#include <graphics/Core.hh>
#include <graphics/Runtime.hh>
#include <graphics/Window.hh>

namespace application {
  std::string readShaderFromFile(const std::string& fileName) {
    std::ifstream fileIn(fileName.c_str());
    if (fileIn.bad() or fileIn.fail()) {
      throw std::invalid_argument("Unable to open shader input file " + fileName);
    }
    std::ostringstream textStream;
    textStream << fileIn.rdbuf();
    fileIn.close();

    return textStream.str();
  }

  void printUsage() {
    std::cout << R"str(
  Usage: shadertest [OPTIONS] <fragmentFilePath>

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

  bool parseArguments(int argc, char** argv, graphics::Configurations& configs) {
    if (argc < 2) {
      throw std::invalid_argument("Please specify a fragment shader file, or pass --info-only or --help.");
    } else {
      for (int a = 1; a < argc; a++) {
        std::string arg(argv[a]);
        if (arg == "--info-only") {
          configs.wantInfoOnly = true;
        } else if (arg == "--help") {
          printUsage();
          return false;
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
            std::tie(configs.wantVersionMajor, configs.wantVersionMinor) = graphics::parseGLVersion(arg.substr(5));
            configs.versionRequest = graphics::VersionRequest::Specific;
          }
        // After all switches/options, assume the rest is the file name.
        } else {
          // If we've already got a file name, then print an error.
          if (not configs.fragmentFilePath.empty()) {
            throw std::invalid_argument("Cannot have more than one fragment shader path.");
          }
          configs.fragmentFilePath = arg;
        }
      }
    }
    return true;
  }

  void printTryingString(const graphics::Configurations& configs) {
    if (configs.wantVersionMajor > 0 or configs.profileRequest != graphics::ProfileRequest::Any) {
      std::cout << "Trying OpenGL";
    
      if (configs.wantVersionMajor > 0) {
        std::cout << " " << configs.wantVersionMajor << "." << configs.wantVersionMinor;
      }
      if (configs.profileRequest == graphics::ProfileRequest::Compat) {
        std::cout << " Compatibility";
      } else if (configs.profileRequest == graphics::ProfileRequest::Core) {
        std::cout << " Core";
      }
      std::cout << std::endl;
    }
  }

  int main(int argc, char** argv) {
    try {
      graphics::Configurations configs;
      if (not parseArguments(argc, argv, configs)) {
        return EXIT_SUCCESS;
      }
      printTryingString(configs);
      graphics::initializeWindow(configs);
      graphics::printInfo();
      if (configs.wantInfoOnly) {
        return EXIT_SUCCESS;
      }

      if (configs.fragmentFilePath.empty()) {
        throw std::invalid_argument("Please specify a fragment shader path");
      }
      configs.fragmentSource = readShaderFromFile(configs.fragmentFilePath);
      configs.vertexSource = graphics::initializeVertexSource(configs.fragmentSource);

      graphics::run(configs);
    } catch (std::exception& e) {
      std::cerr << "\x1b[0;31m" << "Error"
        "\x1b[0m" << ": " << e.what() << std::endl;
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
  }
}

int main(int argc, char** argv) {
  return application::main(argc, argv);
}

