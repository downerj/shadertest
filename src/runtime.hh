#ifndef RUNTIME_HH
#define RUNTIME_HH

#include "gl-includes.hh"

namespace graphics {
struct Configurations;
class WindowHandler;

void run(WindowHandler& windowHandler, const Configurations& configs);
} // namespace graphics

#endif // RUNTIME_HH
