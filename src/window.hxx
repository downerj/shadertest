#ifndef WINDOW_HXX
#define WINDOW_HXX

#include "models.hxx"

struct GLFWwindow;

struct WindowActions {
  bool changeModelType{false};
  ModelType modelType{ModelType::Rectangle};

  auto reset() -> void;
};

class WindowOwner {
public:
  WindowOwner();
  WindowOwner(const WindowOwner&) = delete;
  WindowOwner(WindowOwner&&) = delete;
  WindowOwner operator=(const WindowOwner&) = delete;
  WindowOwner operator=(WindowOwner&&) = delete;
  ~WindowOwner();

  auto getWindow() -> GLFWwindow*;
  auto getActions() -> WindowActions&;
  auto isActive() -> bool;

private:
  GLFWwindow* window;
  const int initialWidth{400};
  const int initialHeight{400};
  const char* title{"ShaderTest"};
  WindowActions actions{};
  auto onKey(
    GLFWwindow* window, int key, int scancode, int action, int mods
  ) -> void;
};

#endif // WINDOW_HXX
