#ifndef WINDOW_HH
#define WINDOW_HH

#include "gl-includes.hh"

namespace graphics {
  struct Configurations;

  auto createWindow(Configurations& configs) -> GLFWwindow*;
  auto initializeWindow(Configurations& configs) -> void;
}

#endif // WINDOW_HH
