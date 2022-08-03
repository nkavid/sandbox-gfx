#pragma once

#include <cstddef>
#include <string_view>

namespace gfx::utils
{
void dump_texture(std::string_view filename,
                  unsigned int texture,
                  size_t width,
                  size_t height);
}
