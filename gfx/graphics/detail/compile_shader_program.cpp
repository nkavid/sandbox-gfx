#include "compile_shader_program.hpp"

#include "utils/logger.hpp"

#include <GL/glew.h>

#include <array>
#include <cstdlib>

namespace gfx::graphics::detail
{
namespace
{
[[noreturn]] void fatalShaderError(unsigned int shader)
{
  constexpr size_t infoLogSize{512};
  std::array<char, infoLogSize> infoLog{};
  glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog.data());
  utils::logger::fatal(infoLog.data());
}

unsigned int compileShader(const char* sourceString, GLenum type)
{
  const unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &sourceString, nullptr);
  glCompileShader(shader);

  int success{};
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (success == GL_FALSE)
  {
    fatalShaderError(shader);
  }

  return shader;
}

[[noreturn]] void fatalProgramError(unsigned int program)
{
  constexpr size_t infoLogSize{512};
  std::array<char, infoLogSize> infoLog{};
  glGetProgramInfoLog(program, infoLogSize, nullptr, infoLog.data());
  utils::logger::fatal(infoLog.data());
}

template <class T>
unsigned int createProgram(const T& shaders)
{
  const unsigned int shaderProgram = glCreateProgram();

  for (const auto& shader : shaders)
  {
    glAttachShader(shaderProgram, shader);
  }

  glLinkProgram(shaderProgram);

  int success{};
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

  if (success == GL_FALSE)
  {
    fatalProgramError(shaderProgram);
  }

  return shaderProgram;
}

namespace stage
{
enum stage
{
  vertex,
  fragment,
  num_stages
};
}
}

unsigned int compileShaderProgram(const char* vertSource, const char* fragSource)
{
  std::array<unsigned int, stage::num_stages> shaders{};
  shaders[stage::vertex]   = compileShader(vertSource, GL_VERTEX_SHADER);
  shaders[stage::fragment] = compileShader(fragSource, GL_FRAGMENT_SHADER);

  const unsigned int shaderProgram = createProgram(shaders);

  for (const auto& shader : shaders)
  {
    glDeleteShader(shader);
  }

  return shaderProgram;
}

unsigned int compileShaderProgram(const char* compSource)
{
  std::array<unsigned int, 1> shaders{};
  shaders[0] = compileShader(compSource, GL_COMPUTE_SHADER);

  const unsigned int shaderProgram = createProgram(shaders);

  glDeleteShader(shaders[0]);

  return shaderProgram;
}
}
