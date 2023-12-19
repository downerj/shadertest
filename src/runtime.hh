#ifndef RUNTIME_HH
#define RUNTIME_HH

#include "gl-includes.hh"

namespace graphics {
  struct Configurations;

  auto run(Configurations& configs) -> void;
}

#endif // RUNTIME_HH
