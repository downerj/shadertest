#ifndef WINDOW_HH
#define WINDOW_HH

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace graphics {
struct Configurations;

struct Position {
  int x{};
  int y{};
};

struct Dimensions {
  int width{};
  int height{};
};

class WindowHandler {
public:
  WindowHandler(Configurations& configs);
  GLFWwindow* getWindow() { return window; }
  bool getIsAnimationPaused() { return isAnimationPaused; }

private:
  GLFWwindow* window;
  Configurations& configs;
  bool isFullScreen{false};
  bool isAnimationPaused{false};
  Position initialPosition{};
  Dimensions initialSize{400, 400};
  Position position;
  Dimensions size;

  void createWindow(const Configurations& configs) ;
  void onKey(int key, int action, int mods);
  void onMove(int x, int y);
  void onResize(int width, int height);
};
} // namespace graphics

#endif // WINDOW_HH
