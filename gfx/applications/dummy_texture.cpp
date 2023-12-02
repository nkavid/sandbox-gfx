#include "applications/dummy_texture.hpp"

#include "vocabulary/color.hpp"
#include "vocabulary/size.hpp"

#include <GL/glew.h>

#include <cstdint>
#include <cstdlib>

namespace gfx::dummy_texture
{
namespace
{
template <class T>
void fillRows(T& data,
              size_t begin,
              size_t end,
              const gfx::Color& color,
              const gfx::Size& size)
{
  auto floatToUint8 = [](float floatData) {
    constexpr static const float maxUint8AsFloat{255.0F};
    return static_cast<uint8_t>(maxUint8AsFloat * floatData);
  };

  for (size_t x = 0; x < size.width; ++x) // NOLINT
  {
    for (size_t y = begin; y < end; ++y) // NOLINT
    {
      data[x * 4 + y * size.width * 4 + 0] = floatToUint8(color.red());   // NOLINT
      data[x * 4 + y * size.width * 4 + 1] = floatToUint8(color.green()); // NOLINT
      data[x * 4 + y * size.width * 4 + 2] = floatToUint8(color.blue());  // NOLINT
      data[x * 4 + y * size.width * 4 + 3] = floatToUint8(color.alpha()); // NOLINT
    }
  }
}
} // namespace

TextureData get()
{
  constexpr gfx::Color red{0xFF0000};
  constexpr gfx::Color white{0xFFFFFF};
  constexpr gfx::Color black{0x000000};
  constexpr gfx::Color blue{0x0000FF};

  TextureData data{};

  // NOLINTBEGIN(readability-magic-numbers)
  constexpr auto quarterHeight = size.height / 4;
  fillRows(data, 0, quarterHeight, white, size);
  fillRows(data, quarterHeight, quarterHeight * 2, blue, size);
  fillRows(data, quarterHeight * 2, quarterHeight * 3, black, size);
  fillRows(data, quarterHeight * 3, quarterHeight * 4, red, size);
  // NOLINTEND(readability-magic-numbers)

  return data;
}

} // namespace gfx::dummy_texture
