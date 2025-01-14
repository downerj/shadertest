#ifndef IO_HXX
#define IO_HXX

#include <optional>
#include <string>
#include <string_view>

auto readFile(std::string_view filePath) -> std::optional<std::string>;

#endif // IO_HXX
