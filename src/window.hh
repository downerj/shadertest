#ifndef WINDOW_HH
#define WINDOW_HH

#include "gl-includes.hh"

namespace graphics {
  class Configurations;

  GLFWwindow* createWindow(Configurations& configs);
  void initializeWindow(Configurations& configs);
}

#endif // WINDOW_HH
