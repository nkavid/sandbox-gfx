#include "shader.hpp"

#include "detail/compile_shader_program.hpp"
#include "vocabulary/color.hpp"

#include <GL/glew.h>

#include <array>
#include <cstddef>

namespace gfx::graphics
{
Shader::Shader(const char* vertex, const char* fragment)
    : _program{detail::compileShaderProgram(vertex, fragment)},
      _colorLoc{glGetUniformLocation(_program, "uColor")},
      _matrixLoc{glGetUniformLocation(_program, "uMatrix")}
{
}

Shader::Shader(const char* compute)
    : _program{detail::compileShaderProgram(compute)}
{
}

void Shader::use() const
{
  glUseProgram(_program);
}

void Shader::setColorUniform(float red, float green, float blue) const
{
  glUniform3f(_colorLoc, red, green, blue);
}

void Shader::setColorUniform(const gfx::Color& color) const
{
  glUniform3f(_colorLoc, color.red(), color.green(), color.blue());
}

void Shader::setMatrixUniform(float scale) const
{
  constexpr size_t matrixSize{9};
  std::array<float, matrixSize> matrix = {
      {scale, 0.0F, 0.0F, 0.0F, scale, 0.0F, 0.0F, 0.0F, 1.0F}
  };

  glUniformMatrix3fv(_matrixLoc, 1, GL_FALSE, matrix.data());
}
}
