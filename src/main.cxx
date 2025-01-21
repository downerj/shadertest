#include <cstdlib>
#include <exception>

#include "debug.hxx"
#include "graphics.hxx"
#include "io.hxx"

auto main(int, char**) -> int {
  try {
    GraphicsEngine graphics{};
    LOG("Running...\n");
    while (graphics.isActive()) {
      graphics.render();
    }
    LOG("Goodbye.\n");
  } catch (std::exception&) {
    std::exit(EXIT_FAILURE);
  }
}
