#ifndef WINDOW_HH
#define WINDOW_HH

#include "gl-includes.hh"

namespace graphics {
  struct Configurations;

  class WindowHandler {
  public:
    WindowHandler(Configurations& configs);
    auto getWindow() -> GLFWwindow* { return window; }
  private:
    GLFWwindow* window;
    Configurations& configs;

    auto createWindow(const Configurations& configs) -> void;
    auto onKey(int key, int action, int mods) -> void;
  };
  
}

#endif // WINDOW_HH
