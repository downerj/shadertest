#ifndef IO_HXX
#define IO_HXX

#include <optional>
#include <string>
#include <string_view>

std::optional<std::string> readFile(std::string_view filePath);

#endif // IO_HXX
