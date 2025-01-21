#include <cstdlib>
#include <exception>

#include "debug.hxx"
#include "graphics.hxx"
// #include "io.hxx"
#include "window.hxx"

auto main(int, char**) -> int {
  try {
    WindowOwner windowOwner{};
    GraphicsEngine graphics{windowOwner.getWindow()};
    LOG("Running...\n");
    while (windowOwner.isActive()) {
      graphics.render();
    }
    LOG("Goodbye.\n");
  } catch (std::exception& ex) {
    std::cerr << ex.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
}
