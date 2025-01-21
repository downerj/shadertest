#ifndef WINDOW_HXX
#define WINDOW_HXX

struct GLFWwindow;

class WindowOwner {
public:
  WindowOwner();
  WindowOwner(const WindowOwner&) = delete;
  WindowOwner(WindowOwner&&) = delete;
  WindowOwner operator=(const WindowOwner&) = delete;
  WindowOwner operator=(WindowOwner&&) = delete;
  ~WindowOwner();

  auto getWindow() -> GLFWwindow*; 
  auto isActive() -> bool;

private:
  GLFWwindow* window;
  auto onKey(
    GLFWwindow* window, int key, int scancode, int action, int mods
  ) -> void;
};

#endif // WINDOW_HXX
