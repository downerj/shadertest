#ifndef WINDOW_HH
#define WINDOW_HH

#include "gl-includes.hh"

namespace graphics {
  struct Configurations;

  struct Position {
    int x = 0;
    int y = 0;
  };

  struct Dimensions {
    int width = 0;
    int height = 0;
  };

  class WindowHandler {
  public:
    WindowHandler(Configurations& configs);
    auto getWindow() -> GLFWwindow* { return window; }
  private:
    GLFWwindow* window;
    Configurations& configs;
    bool isFullScreen = false;
    Position position{};
    Dimensions dimensions{};

    auto createWindow(const Configurations& configs) -> void;
    auto onKey(int key, int action, int mods) -> void;
    auto onMove(int x, int y) -> void;
    auto onResize(int width, int height) -> void;
  };
}

#endif // WINDOW_HH
