#ifndef RUNTIME_HH
#define RUNTIME_HH

#include "gl-includes.hh"

namespace graphics {
struct Configurations;
class WindowHandler;

auto run(WindowHandler& windowHandler, const Configurations& configs) -> void;
} // namespace graphics

#endif // RUNTIME_HH
