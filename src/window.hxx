#ifndef WINDOW_HXX
#define WINDOW_HXX

#include "models.hxx"

struct GLFWwindow;

struct WindowActions {
  bool changeModelType{false};
  ModelType modelType{ModelType::Rectangle};
  bool closeWindow{false};
  bool resetWindowSize{false};
  bool pauseResume{false};

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
  auto closeWindow() -> void;
  auto resetWindowSize() -> void;
  auto update() -> void;

private:
  GLFWwindow* _window;
  const int _initialWidth{400};
  const int _initialHeight{400};
  const char* _title{"ShaderTest"};
  WindowActions _actions{};

  static auto onKeyGLFW(
    GLFWwindow* window, int key, int scancode, int action, int mods
  ) -> void;
  auto onKey(
    int key, int action, int mods
  ) -> void;
};

#endif // WINDOW_HXX
