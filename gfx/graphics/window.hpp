#pragma once

#include "vocabulary/size.hpp"

#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace gfx::graphics
{
enum class WindowMode
{
  Windowed,
  Headless
};

class Window
{
  public:
    using Mode = WindowMode;
    Window(const char* name, const gfx::Size& size, Mode mode = Mode::Windowed);
    ~Window();
    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&)                 = delete;
    Window& operator=(Window&&)      = delete;

    void swap();
    [[nodiscard]] bool isOpen() const;

  private:
    gfx::Size _size;
    GLFWwindow* _window{nullptr};
};
}
