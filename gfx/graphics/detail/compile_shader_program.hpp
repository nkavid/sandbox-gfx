#pragma once

namespace gfx::graphics::detail
{
unsigned int compileShaderProgram(const char* vertSource, const char* fragSource);
unsigned int compileShaderProgram(const char* compSource);
} // namespace gfx::graphics::detail
