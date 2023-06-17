#include "window.hpp"

#include "utils/logger.hpp"

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <iostream>

namespace gfx::graphics
{
namespace
{
void glfw_error_callback(int /*error*/, const char* description)
{
  std::cout << "gfx::window: " << description << '\n';
}

// NOLINTNEXTLINE
void APIENTRY opengl_debug_callback(GLenum source,
                                    GLenum type,
                                    unsigned int message_id,
                                    GLenum severity,
                                    GLsizei /*length*/,
                                    const char* message,
                                    const void* /*userParam*/)
{
  std::cout << "-------opengl_debug_callback--------" << '\n';
  std::cout << "Debug message (" << message_id << "): " << message << '\n';

  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
    std::cout << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cout << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cout << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cout << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cout << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cout << "Source: Other";
    break;
  }
  std::cout << '\n';

  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "Type: Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "Type: Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cout << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cout << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cout << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "Type: Other";
    break;
  }
  std::cout << '\n';

  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "Severity: notification";
    break;
  }
  std::cout << "\n\n";
}

struct OpenglVersion
{
    constexpr static int major{4};
    constexpr static int minor{6};
};

void initialize_glfw(Window::Mode mode)
{
  glfwSetErrorCallback(glfw_error_callback);

  if (glfwInit() == GLFW_FALSE)
  {
    utils::logger::fatal("graphics::Window - Failed to call glfwInit()");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OpenglVersion::major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OpenglVersion::minor);

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
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

void initialize_opengl()
{
  glewExperimental = GL_TRUE;
  glewInit();

  int flags{0};
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if ((flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0)
  {
    std::cout << "-- OPENGL DEBUG OUTPUT ENABLED\n";
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE,
                          GL_DONT_CARE,
                          GL_DONT_CARE,
                          0,
                          nullptr,
                          GL_TRUE);
  }

  glClearColor(1.0F, 1.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLFWwindow* create_window_glfw(const char* name,
                               const gfx::Size& size,
                               Window::Mode mode)
{
  initialize_glfw(mode);
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

  initialize_opengl();

  return window;
}

} // namespace

Window::Window(const char* name, const gfx::Size& size, Mode mode)
    : _window{create_window_glfw(name, size, mode)}
{}

Window::~Window()
{
  glfwMakeContextCurrent(nullptr);
  glfwDestroyWindow(_window);
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
} // namespace gfx::graphics
