#include "io.hxx"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "debug.hxx"

std::optional<std::string> readFile(std::string_view filePath) {
  if (!std::filesystem::exists(filePath)) {
    LOG_ERROR("File does not exist: " << filePath << '\n');
    return {};
  }
  try {
    std::ifstream streamIn{filePath.data()};
    std::ostringstream streamOut{};
    std::string lineBuffer{};
    while (std::getline(streamIn, lineBuffer)) {
      streamOut << lineBuffer << '\n';
    }
    return streamOut.str();
  } catch (const std::exception& ex) {
    LOG_ERROR("Error reading from file: " << filePath << '\n');
    LOG_ERROR("Caught error: " << ex.what() << '\n');
    return {};
  }
}
