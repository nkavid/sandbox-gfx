#pragma once

#include "vocabulary/size.hpp"

#include <GL/glew.h>

#include <array>
#include <cstdint>

namespace gfx::dummy_texture
{
constexpr gfx::Size size{gfx::Width{512}, gfx::Height{512}};
constexpr unsigned int channels{4};
using TextureData = std::array<uint8_t, size.width * size.height * channels>;

TextureData get();
} // namespace gfx::dummy_texture
