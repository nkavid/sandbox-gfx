#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "graphics/utils/graphics_dump.hpp"
#include "graphics/window.hpp"
#include "shaders/yuv.hpp"
#include "vocabulary/color.hpp"
#include "vocabulary/size.hpp"

#include <GL/glew.h>

#include <array>
#include <cstdint>
#include <cstdlib>

namespace gfx
{
template <class T>
static void fillRows(T& data,
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
}

int main()
{
  using namespace gfx;
  constexpr gfx::Size size{gfx::Width{512}, gfx::Height{512}};

  const graphics::Window window{"opengl_compute_shader",
                                size,
                                graphics::Window::Mode::Headless};

  constexpr unsigned int channels{4};
  std::array<uint8_t, size.width * size.height * channels> data{};

  constexpr gfx::Color red{0xFF0000};
  constexpr gfx::Color white{0xFFFFFF};
  constexpr gfx::Color black{0x000000};
  constexpr gfx::Color blue{0x0000FF};

  // NOLINTBEGIN(readability-magic-numbers)
  constexpr auto quarterHeight = size.height / 4;
  fillRows(data, 0, quarterHeight, white, size);
  fillRows(data, quarterHeight, quarterHeight * 2, blue, size);
  fillRows(data, quarterHeight * 2, quarterHeight * 3, black, size);
  fillRows(data, quarterHeight * 3, quarterHeight * 4, red, size);
  // NOLINTEND(readability-magic-numbers)

  const graphics::Texture inputTexture{size, data.data()};
  const graphics::Texture outputTexture{size};

  const graphics::Shader shader{graphics::shaders::yuv::compute};

  shader.use();

  glBindImageTexture(0, inputTexture.get(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
  glBindImageTexture(1, outputTexture.get(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

  glDispatchCompute(size.width, size.height, 1);

  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glActiveTexture(GL_TEXTURE1);
  utils::dump_texture("output.jpg", outputTexture.get(), size.width, size.height);
  glActiveTexture(GL_TEXTURE0);
  utils::dump_texture("input.jpg", inputTexture.get(), size.width, size.height);

  return EXIT_SUCCESS;
}
