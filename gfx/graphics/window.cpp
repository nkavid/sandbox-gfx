#include "window.hpp"

#include "utils/logger.hpp"

#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace gfx::graphics
{
namespace
{
namespace opengl_version
{
constexpr int major{4};
constexpr int minor{6};
}

void initialize_glfw(Window::Mode mode)
{
  if (glfwInit() == GLFW_FALSE)
  {
    utils::logger::fatal("graphics::Window - Failed to call glfwInit()");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_version::major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_version::minor);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

  if (mode == Window::Mode::Headless)
  {
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  }
  else
  {
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  }
}

GLFWwindow* create_window_glfw(const char* name, const gfx::Size& size)
{
  auto* window = glfwCreateWindow(size.width, size.height, name, nullptr, nullptr);

  if (window == nullptr)
  {
    utils::logger::fatal("graphics::Window - Failed to Create OpenGL Context");
  }

  glfwSetWindowAttrib(window, GLFW_AUTO_ICONIFY, GLFW_TRUE);

  glfwMakeContextCurrent(window);

  // NOLINTBEGIN(bugprone-easily-swappable-parameters)
  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow*, int widthParam, int heightParam) {
                                   glViewport(0, 0, widthParam, heightParam);
                                 });
  // NOLINTEND(bugprone-easily-swappable-parameters)

  return window;
}

void initialize_opengl()
{
  glewExperimental = GL_TRUE;
  glewInit();

  glClearColor(1.0F, 1.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

}

Window::Window(const char* name, const gfx::Size& size, Mode mode)
    : _size{size.width, size.height}
{
  initialize_glfw(mode);
  // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
  _window = create_window_glfw(name, _size);
  initialize_opengl();
}

Window::~Window()
{
  glfwTerminate();
}

void Window::swap()
{
  if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(_window, GLFW_TRUE);
  }
  glfwSwapBuffers(_window);
  glfwPollEvents();
  glClear(GL_COLOR_BUFFER_BIT);
}

bool Window::isOpen() const
{
  return glfwWindowShouldClose(_window) == GLFW_FALSE;
}
}